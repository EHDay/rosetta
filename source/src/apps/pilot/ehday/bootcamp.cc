// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

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

using namespace core::scoring;
int main( int argc, char ** argv) {
	devel::init( argc, argv ) ;
	utility::vector1< std::string > filenames = basic::options::option[
	basic::options::OptionKeys::in::file::s ].value();
	if ( filenames.size() > 0 ) {
		std::cout << "You entered: " << filenames[ 1 ] << " as the PDB file to be read" << std::endl;
	}
	else {
		std::cout << "You didn't provide a PDB file with the -in::file::s option" << std::endl;
		return 1;
	}
	
	core::pose::PoseOP mypose = core::import_pose::pose_from_file( filenames[1] );
	ScoreFunctionOP sfxn = get_score_function();
	
	core::Real score = sfxn -> score(*mypose);
	std::cout << "Initial score: " << score << std::endl;
	
	core::Size seqpos;
	protocols::moves::MonteCarlo mc= protocols::moves::MonteCarlo(* mypose, * sfxn, 1.0);
	seqpos = mypose->size();
	std::cout << "Number of residues: " << seqpos << std::endl;
	
	protocols::moves::PyMOLObserverOP the_observer = protocols::moves::AddPyMOLObserver( *mypose, true, 0 );
	the_observer->pymol().apply( *mypose);

	core::kinematics::MoveMap mm;
	mm.set_bb( true );
	mm.set_chi( true);

	core::optimization::MinimizerOptions min_opts( "lbfgs_armijo_atol", 0.01, true );
	
	core::optimization::AtomTreeMinimizer atm;
	core::pose::Pose copy_pose;
	core::Real score_avg = 0;
	core::Size accepted = 0;
	core::Size not_accepted = 0;
	for (core::Size i = 1; i <= 201; ++i) {
		//std::cout << i << std::endl;
		
		core::Real uniform_random_number = numeric::random::uniform();
		core::Real pert1 = numeric::random::gaussian();
		core::Real pert2 = numeric::random::gaussian();
		
		//std::cout << uniform_random_number << std::endl;
		
		core::Size randres = static_cast< core::Size > ( uniform_random_number * seqpos + 1 );
		//std::cout << randres << std::endl;
		
		core::Real orig_phi = mypose->phi( randres );
		core::Real orig_psi = mypose->psi( randres );
		mypose->set_phi( randres, orig_phi + pert1 );
		mypose->set_psi( randres, orig_psi + pert2 );
		core::pack::task::PackerTaskOP repack_task = core::pack::task::TaskFactory::create_packer_task( *mypose );
		repack_task->restrict_to_repacking();
		core::pack::pack_rotamers( *mypose, *sfxn, repack_task );
		copy_pose = *mypose;
		atm.run( copy_pose, mm, *sfxn, min_opts );
		*mypose = copy_pose;
		//std::cout << "total_score: " <<  mc.total_score_of_last_considered_pose();
		bool boltzmann_pass = mc.boltzmann (* mypose );
		score_avg += mc.last_score();
		if (boltzmann_pass) {
			accepted += 1;
		}
		else {
			not_accepted += 1;
		}
		//std::cout << "total_score: " <<  mc.total_score_of_last_considered_pose();
		//std::cout << "total_trials: " << mc.total_trials();
		std::cout <<accepted<< std::endl;
		std::cout << not_accepted<<std::endl;
		if (i % 100 == 0) {
			core::Real acceptance_rate = (core::Real) accepted / (core::Real) i;
			std::cout << "Acceptance Rate" << acceptance_rate << std::endl;
			std::cout << "Score average" << (score_avg/i) << std::endl;
		}
	}
	return 0;
}
