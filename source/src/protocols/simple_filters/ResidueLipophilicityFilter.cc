// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file protocols/simple_filters/ResidueLipophilicityFilter.cc
/// @brief returns the value calculated by the energy term mp_res_lipo.
/// useful for understanding and debugging this energy term. use the output_file tag
/// to print a full table of parameters from within the energy term
/// @author Jonathan Weinstein (jonathan.weinstein@weizmann.ac.il)
// Project Headers

#include <basic/Tracer.hh>
#include <core/pose/Pose.fwd.hh>
#include <core/types.hh>
#include <basic/datacache/DataMap.fwd.hh>
#include <protocols/simple_filters/ResidueLipophilicityFilter.hh>
#include <protocols/simple_filters/ResidueLipophilicityFilterCreator.hh>
#include <string>
#include <utility/tag/Tag.hh>

#include <protocols/jd2/JobDistributor.hh>
#include <fstream>
#include <core/energy_methods/MPResidueLipophilicityEnergy.hh>
#include <utility/tag/XMLSchemaGeneration.hh>
#include <protocols/filters/filter_schemas.hh>

namespace protocols {
namespace simple_filters {

static basic::Tracer TR( "protocols.simple_filters.ResidueLipophilicityFilter" );

protocols::filters::FilterOP
ResidueLipophilicityFilterCreator::create_filter() const { return utility::pointer::make_shared< ResidueLipophilicityFilter >(); }

std::string
ResidueLipophilicityFilterCreator::keyname() const { return "ResidueLipophilicity"; }

ResidueLipophilicityFilter::~ResidueLipophilicityFilter(){}

void
ResidueLipophilicityFilter::parse_my_tag(
	utility::tag::TagCOP tag,
	basic::datacache::DataMap &
) {
	threshold_ = tag->getOption< core::Real >( "threshold", -10.0 );
	output_ = tag->getOption< std::string >( "output_file", "TR" );
	print_splines_ = tag->getOption< bool >( "print_splines", false );

	TR << "testing ResidueLipophilicity with threshold " << threshold_ << std::endl;
}

bool
ResidueLipophilicityFilter::apply( core::pose::Pose const & pose ) const {

	core::Real total_mp_res_lipo = compute( pose );

	bool const status = total_mp_res_lipo <= threshold_;

	TR << "ResidueLipophilicity threshold is at " << threshold_ << " total ResidueLipophilicity is " << total_mp_res_lipo << std::endl;

	return status;
}

void
ResidueLipophilicityFilter::report( std::ostream & out, core::pose::Pose const & pose ) const {
	core::Real total_mp_res_lipo = compute( pose );
	out << "total res solv: " << total_mp_res_lipo << std::endl;
}

core::Real
ResidueLipophilicityFilter::report_sm( core::pose::Pose const & pose ) const {
	core::Real total_mp_res_lipo = compute( pose );
	return( total_mp_res_lipo );
}

core::Real
ResidueLipophilicityFilter::compute( core::pose::Pose const & pose ) const {
	core::energy_methods::MPResidueLipophilicityEnergy mp_mp_res_lipo;
	core::Real total_mp_res_lipo = 0.0;

	if ( output_ == "TR" ) {
		total_mp_res_lipo = mp_mp_res_lipo.report_ressolv( TR, pose, print_splines_ );
	} else if ( output_ == "auto" ) {
		std::ofstream outfile;
		std::string file_name = protocols::jd2::JobDistributor::get_instance()->current_output_name() + "_RS.txt";
		TR << "printing report to " << file_name << std::endl;
		outfile.open( file_name, std::ios::out  );
		total_mp_res_lipo = mp_mp_res_lipo.report_ressolv( outfile, pose, print_splines_ );
	} else {
		std::ofstream outfile;
		outfile.open( output_.c_str(), std::ios::out );
		total_mp_res_lipo = mp_mp_res_lipo.report_ressolv( outfile, pose, print_splines_ );
	}
	return( total_mp_res_lipo );
}

void ResidueLipophilicityFilter::provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd )
{
	using namespace utility::tag;
	AttributeList attlist;
	attlist + XMLSchemaAttribute::attribute_w_default( "threshold" , xsct_real , "if ResidueLipophilicity score is lower than threshold, than pass." , "-10" )
		+ XMLSchemaAttribute::attribute_w_default( "output_file" , xs_string , "where to print the result to. default to TRACER. use auto for job_name_RS.txt" , "TR" )
		+ XMLSchemaAttribute::attribute_w_default( "print_splines" , xsct_rosetta_bool , "whehter to print the splines, default=false" , "false" );
	protocols::filters::xsd_type_definition_w_attributes( xsd, class_name(), "Computes the overall ResidueLipophilicity of the pose.", attlist );
}

void ResidueLipophilicityFilterCreator::provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd ) const
{
	ResidueLipophilicityFilter::provide_xml_schema( xsd );
}

std::string ResidueLipophilicityFilter::name() const {
	return class_name();
}

std::string ResidueLipophilicityFilter::class_name() {
	return "ResidueLipophilicity";
}

}
}
