// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file   protocols/features/DdGFeatures.cc
/// @brief  report the per-residue ddG score to the features database
/// @author Kyle Barlow (kb@kylebarlow.com)

// Unit Headers
#include <protocols/features/DdGFeatures.hh>

// Project Headers
#include <protocols/filters/Filter.fwd.hh>
#include <core/pose/Pose.fwd.hh>
#include <core/scoring/ScoreFunction.fwd.hh>
#include <core/types.hh>
#include <basic/datacache/DataMap.fwd.hh>
#include <basic/database/sql_utils.hh>
#include <basic/Tracer.hh>
#include <basic/database/schema_generator/PrimaryKey.hh>
#include <basic/database/schema_generator/ForeignKey.hh>
#include <basic/database/schema_generator/Column.hh>
#include <basic/database/schema_generator/Schema.hh>
#include <basic/database/schema_generator/DbDataType.hh>
#include <protocols/rosetta_scripts/util.hh>
#include <protocols/simple_ddg/DdGScan.hh>

// Utility Headers
#include <utility/tag/Tag.hh>
#include <utility/vector1.hh>

// External Headers
#include <cppdb/frontend.h>

// C++ Headers
#include <utility/excn/Exceptions.hh>
#include <sstream>
// XSD XRW Includes
#include <utility/tag/XMLSchemaGeneration.hh>
#include <protocols/features/feature_schemas.hh>
#include <protocols/features/DdGFeaturesCreator.hh>

#include <core/scoring/EnergyMap.fwd.hh> // AUTO IWYU For EnergyMap

namespace protocols {
namespace features {

using std::endl;
using std::string;
using std::stringstream;
using core::Size;
using core::Real;
using core::pose::Pose;
using core::pose::PoseOP;
using core::scoring::EnergyMap;
using core::scoring::ScoreFunctionOP;
// using core::scoring::ScoreTypeManager;
// using core::scoring::ScoreTypes;
using basic::datacache::DataMap;
using utility::tag::TagCOP;
using utility::vector1;
using basic::database::safely_write_to_database;
using utility::sql_database::sessionOP;
using cppdb::statement;

static basic::Tracer TR( "protocols.features.DdGFeatures" );

DdGFeatures::DdGFeatures() = default;

DdGFeatures::DdGFeatures(
	protocols::simple_ddg::DdGScanOP ddG_scan_mover
)
{
	ddG_scan_mover_ = ddG_scan_mover;
}

DdGFeatures::DdGFeatures( DdGFeatures const & ) = default;

DdGFeatures::~DdGFeatures() = default;


void
DdGFeatures::write_schema_to_db(
	sessionOP db_session
) const {
	write_ddG_table_schema(db_session);
}

void
DdGFeatures::write_ddG_table_schema(
	sessionOP db_session
) const {
	using namespace basic::database::schema_generator;

	Column struct_id("struct_id", utility::pointer::make_shared< DbBigInt >());
	Column resNum("resNum", utility::pointer::make_shared< DbInteger >());
	Column mutated_to_name3("mutated_to_name3", utility::pointer::make_shared< DbText >(3));
	Column ddG_value("ddG_value", utility::pointer::make_shared< DbReal >());

	Columns primary_key_columns;
	primary_key_columns.push_back(struct_id);
	primary_key_columns.push_back(resNum);
	primary_key_columns.push_back(mutated_to_name3);
	PrimaryKey primary_key(primary_key_columns);

	Columns foreign_key_columns;
	foreign_key_columns.push_back(struct_id);
	foreign_key_columns.push_back(resNum);
	vector1< string > reference_columns;
	reference_columns.push_back("struct_id");
	reference_columns.push_back("resNum");
	ForeignKey foreign_key(foreign_key_columns, "residues", reference_columns, true);

	// Table name doesn't have a capital G because some other database utility code would
	//  change the table nane "ddG" to "ddg" in MySQL
	Schema table("ddg", primary_key);
	table.add_foreign_key(foreign_key);
	table.add_column(ddG_value);

	table.write(db_session);
}

utility::vector1<string>
DdGFeatures::features_reporter_dependencies() const {
	utility::vector1<string> dependencies;
	dependencies.push_back("ResidueFeatures");
	return dependencies;
}

void
DdGFeatures::parse_my_tag(
	TagCOP const tag,
	basic::datacache::DataMap & data
) {
	if ( tag->hasOption("ddG_scan_mover") ) {
		filters::FilterOP filter = protocols::rosetta_scripts::parse_filter( tag->getOption< std::string >( "ddG_scan_mover" ), data );
		ddG_scan_mover_ = utility::pointer::dynamic_pointer_cast < protocols::simple_ddg::DdGScan > (filter);
	} else {
		stringstream error_msg;
		error_msg
			<< "The " << type_name() << " reporter requires a 'ddG_scan_mover' tag:" << endl
			<< endl
			<< "    <feature name=" << type_name() <<" ddG_scan_mover=(name_of_previously_defined_DdgScanMover) />" << endl;
		throw CREATE_EXCEPTION(utility::excn::RosettaScriptsOptionError, error_msg.str());
	}
}

Size
DdGFeatures::report_features(
	Pose const & pose,
	vector1< bool > const & relevant_residues,
	StructureID const struct_id,
	sessionOP db_session
){

	insert_ddG_rows(pose, relevant_residues, struct_id, db_session );

	return 0;
}

/// @details
void
DdGFeatures::insert_ddG_rows(
	Pose const & pose,
	vector1< bool > const & relevant_residues,
	StructureID const struct_id,
	sessionOP db_session
) const {
	utility::vector1< ddG_data_tuple > ddG_data = ddG_scan_mover_->calculate( TR, pose );

	string const stmt_string("INSERT INTO ddg ( struct_id, resNum, mutated_to_name3, ddG_value ) VALUES (?,?,?,?);");

	statement stmt(
		basic::database::safely_prepare_statement(stmt_string, db_session)
	);

	core::Size resNum; std::string resname; core::Real ddG_value;
	for ( utility::vector1<ddG_data_tuple>::const_iterator iter = ddG_data.begin(), iter_end = ddG_data.end() ; iter != iter_end ; ++iter ) {
		boost::tie(resNum, resname, ddG_value) = *iter;
		if ( !check_relevant_residues( relevant_residues, resNum ) ) continue;

		stmt.bind(1, struct_id);
		stmt.bind(2, resNum);
		stmt.bind(3, resname);
		stmt.bind(4, ddG_value);
		safely_write_to_database(stmt);

	}

} // End function body

std::string DdGFeatures::type_name() const {
	return class_name();
}

std::string DdGFeatures::class_name() {
	return "DdGFeatures";
}

void DdGFeatures::provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd )
{
	using namespace utility::tag;
	AttributeList attlist;
	attlist + XMLSchemaAttribute::required_attribute( "ddG_scan_mover", xs_string, "Mover with which to scan mutations" );

	protocols::features::xsd_type_definition_w_attributes( xsd, class_name(), "Record features of the ddgs for a set of mutations, performed with a particular Mover", attlist );
}

std::string DdGFeaturesCreator::type_name() const {
	return DdGFeatures::class_name();
}

protocols::features::FeaturesReporterOP
DdGFeaturesCreator::create_features_reporter() const {
	return utility::pointer::make_shared< DdGFeatures >();
}

void DdGFeaturesCreator::provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd ) const
{
	DdGFeatures::provide_xml_schema( xsd );
}



} // namespace
} // namespace
