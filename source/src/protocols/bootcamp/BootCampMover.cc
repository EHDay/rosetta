// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part o/f the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file BootCampMover/BootCampMover.cc
/// @brief Bootcamp_pilot_app_mover
/// @author Elijah Day (elijahday2021@gmail.com)

// Unit headers
#include <protocols/bootcamp/BootCampMover.hh>
#include <protocols/bootcamp/BootCampMoverCreator.hh>

// Core headers
#include <core/pose/Pose.hh>

// Basic/Utility headers
#include <basic/Tracer.hh>
#include <utility/tag/Tag.hh>
#include <utility/pointer/memory.hh>

// XSD Includes
#include <utility/tag/XMLSchemaGeneration.hh>
#include <protocols/moves/mover_schemas.hh>

#include <utility/vector1.hh>
#include <basic/citation_manager/UnpublishedModuleInfo.hh>

#include <iostream>
#include <basic/options/option.hh>
#include <basic/options/keys/in.OptionKeys.gen.hh>
#include <devel/init.hh>
#include <core/import_pose/import_pose.hh>
#include <utility/pointer/owning_ptr.hh>

#include <core/pose/Pose.hh>

#include <core/scoring/ScoreFunctionFactory.hh>

#include <core/scoring/ScoreFunction.hh>
#include <utility/options/OptionCollection.hh>

#include <numeric/random/random.hh>
#include <protocols/moves/MonteCarlo.hh>

#include<protocols/moves/PyMOLMover.hh>
#include<core/pack/task/PackerTask.hh>
#include<core/pack/task/TaskFactory.hh>
#include<core/pack/pack_rotamers.hh>

#include<core/kinematics/MoveMap.hh>
#include<core/optimization/MinimizerOptions.hh>
#include<core/optimization/AtomTreeMinimizer.hh>

#include <core/scoring/dssp/Dssp.hh>
#include <core/kinematics/FoldTree.hh>
#include <protocols/bootcamp/fold_tree_from_ss.hh>
#include <core/pose/variant_util.hh>
#include <protocols/jd2/JobDistributor.hh>
#include <devel/init.hh>
#include <protocols/moves/Mover.hh>

static basic::Tracer TR( "protocols.bootcamp.BootCampMover" );

namespace protocols {
namespace bootcamp {

	/////////////////////
	/// Constructors  ///
	/////////////////////

/// @brief Default constructor
BootCampMover::BootCampMover():
	protocols::moves::Mover( BootCampMover::mover_name() )
{}

////////////////////////////////////////////////////////////////////////////////
/// @brief Destructor (important for properly forward-declaring smart-pointer members)
BootCampMover::~BootCampMover() = default;

////////////////////////////////////////////////////////////////////////////////
	/// Mover Methods ///
	/////////////////////

/// @brief Apply the mover
void
BootCampMover::apply( core::pose::Pose& mypose){
	using namespace core::scoring;
	ScoreFunctionOP sfxn = get_score_function();
	sfxn ->set_weight( core::scoring::linear_chainbreak, 1.0); // Lab 4
	core::Real score = sfxn -> score(mypose);

	TR << "Initial score: " << score << std::endl;

	core::Size seqpos;
	protocols::moves::MonteCarlo mc= protocols::moves::MonteCarlo( mypose, * sfxn, 1.0);
	
	seqpos = mypose.size();
	TR << "Number of residues:  " << seqpos << std::endl;


	core::kinematics::MoveMap mm;
	mm.set_bb( true );
	
	mm.set_chi( true);

	core::optimization::MinimizerOptions min_opts( "lbfgs_armijo_atol", 0.01, true );

	core::optimization::AtomTreeMinimizer atm;
	core::pose::Pose copy_pose;
	core::Real score_avg = 0;
	core::Size accepted = 0;
	core::Size not_accepted = 0;

	core::kinematics::FoldTree ft = protocols::bootcamp::fold_tree_from_ss(mypose); // Lab 4
	core::pose::correctly_add_cutpoint_variants(mypose);
	for (core::Size i = 1; i <= 200;++i) {

		core::Real uniform_random_number = numeric::random::uniform();
		core::Real pert1 = numeric::random::gaussian();
		core::Real pert2 = numeric::random::gaussian();

		core::Size randres = static_cast< core::Size > ( uniform_random_number * seqpos + 1 );

		core::Real orig_phi = mypose.phi( randres );
		core::Real orig_psi = mypose.psi( randres );
		mypose.set_phi( randres, orig_phi + pert1 );
		mypose.set_psi( randres, orig_psi + pert2 );
		core::pack::task::PackerTaskOP repack_task = core::pack::task::TaskFactory::create_packer_task( mypose );
		repack_task->restrict_to_repacking();
		core::pack::pack_rotamers( mypose, *sfxn, repack_task );
		copy_pose = mypose;
		atm.run( copy_pose, mm, *sfxn, min_opts );
		mypose = copy_pose;
		bool boltzmann_pass = mc.boltzmann ( mypose );
		score_avg += mc.last_score();
		if (boltzmann_pass) {
			accepted += 1;
		}
		else {
			not_accepted += 1;
		}
		TR <<accepted<< std::endl;
		TR << not_accepted<<std::endl;

		if (i % 100 == 0) {
			core::Real acceptance_rate = (core::Real) accepted / (core::Real) i;
			TR << "Acceptance Rate" << acceptance_rate << std::endl;
			TR << "Score average" << (score_avg/i) << std::endl;
		}
	}
}
//}
//}
////////////////////////////////////////////////////////////////////////////////
/// @brief Show the contents of the Mover
void
BootCampMover::show(std::ostream & output) const
{
	protocols::moves::Mover::show(output);
}

////////////////////////////////////////////////////////////////////////////////
	/// Rosetta Scripts Support ///
	///////////////////////////////

/// @brief parse XML tag (to use this Mover in Rosetta Scripts)
void
BootCampMover::parse_my_tag(
	utility::tag::TagCOP ,
	basic::datacache::DataMap&
) {

}
void BootCampMover::provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd )
{

	using namespace utility::tag;
	AttributeList attlist;

	//here you should write code to describe the XML Schema for the class.  If it has only attributes, simply fill the probided AttributeList.

	protocols::moves::xsd_type_definition_w_attributes( xsd, mover_name(), "Bootcamp_pilot_app_mover", attlist );
}


////////////////////////////////////////////////////////////////////////////////
/// @brief required in the context of the parser/scripting scheme
protocols::moves::MoverOP
BootCampMover::fresh_instance() const
{
	return utility::pointer::make_shared< BootCampMover >();
}

/// @brief required in the context of the parser/scripting scheme
protocols::moves::MoverOP
BootCampMover::clone() const
{
	return utility::pointer::make_shared< BootCampMover >( *this );
}

std::string BootCampMover::get_name() const {
	return mover_name();
}

std::string BootCampMover::mover_name() {
	return "BootCampMover";
}



/////////////// Creator ///////////////

protocols::moves::MoverOP
BootCampMoverCreator::create_mover() const
{
	return utility::pointer::make_shared< BootCampMover >();
}

std::string
BootCampMoverCreator::keyname() const
{
	return BootCampMover::mover_name();
}

void BootCampMoverCreator::provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd ) const
{
	BootCampMover::provide_xml_schema( xsd );
}

/// @brief This mover is unpublished.  It returns Elijah Day as its author.
void
BootCampMover::provide_citation_info(basic::citation_manager::CitationCollectionList & citations ) const {
	citations.add(
		utility::pointer::make_shared< basic::citation_manager::UnpublishedModuleInfo >(
		"BootCampMover", basic::citation_manager::CitedModuleType::Mover,
		"Elijah Day",
		"TODO: institution",
		"elijahday2021@gmail.com",
		"Wrote the BootCampMover."
		)
	);
}


////////////////////////////////////////////////////////////////////////////////
	/// private methods ///
	///////////////////////


std::ostream &
operator<<( std::ostream & os, BootCampMover const & mover )
{
	mover.show(os);
	return os;
}
}
} //BootCampMover
