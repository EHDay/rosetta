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

#include <core/scoring/dssp/Dssp.hh>
#include <core/kinematics/FoldTree.hh>
#include <protocols/bootcamp/fold_tree_from_ss.hh>
#include <core/pose/variant_util.hh>
#include <protocols/jd2/JobDistributor.hh>
#include <devel/init.hh>
#include <protocols/bootcamp/BootCampMover.hh>
#include <protocols/bootcamp/BootCampMover.fwd.hh>
//using namespace core::scoring;
int main(int argc, char ** argv) {
devel::init( argc, argv ) ;
	utility::vector1< std::string > filenames = basic::options::option[
	basic::options::OptionKeys::in::file::s ].value();
	if ( filenames.size() > 0 ) {
		std::cout << "You entered (DEBUG): " << filenames[ 1 ] << " as the PDB file to be read" << std::endl;
	}
	else {
		std::cout << "You didn't provide a PDB file with the -in::file::s option" << std::endl;
		return 1;
	}
	core::pose::PoseOP mypose = core::import_pose::pose_from_file( filenames[1] );
	protocols::moves::PyMOLObserverOP the_observer = protocols::moves::AddPyMOLObserver( *mypose, true, 0 );
	//the_observer->pymol().apply( *mypose);	
	protocols::bootcamp::BootCampMoverOP boot_mover ( new protocols::bootcamp::BootCampMover);
//( new protocols::bootcamp::BootCampMover );
	protocols::jd2::JobDistributor::get_instance()->go(boot_mover);
	
	


	

	return 0;
}
