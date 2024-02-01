// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file  protocols/bootcamp/BootCampMoverTest.cxxtest.hh
/// @brief  Tests for BootCampMover
/// @author Elijah Day (elijahday2021@gmail.com)

//#include<iostream>
// Test headers
#include <test/UMoverTest.hh>
#include <test/UTracer.hh>
#include <cxxtest/TestSuite.h>
#include <test/util/pose_funcs.hh>
#include <test/core/init_util.hh>

// Project Headers


// Core Headers
#include <core/pose/Pose.hh>
#include <core/import_pose/import_pose.hh>

// Utility, etc Headers
#include <basic/Tracer.hh>
// Headers from BootCampMover.cc
#include <protocols/bootcamp/BootCampMover.hh>
#include <protocols/bootcamp/BootCampMover.fwd.hh>
#include <protocols/bootcamp/BootCampMoverCreator.hh>
#include <protocols/moves/Mover.hh>
#include <protocols/moves/MoverFactory.hh>
#include <protocols/jd2/JobDistributor.hh>
#include <utility/pointer/owning_ptr.hh>
#include <core/scoring/ScoreFunctionFactory.hh>

#include <core/scoring/ScoreFunction.hh>
#include <utility/options/OptionCollection.hh>





static basic::Tracer TR("BootCampMoverTest");


class BootCampMoverTest : public CxxTest::TestSuite {
	//Define Variables

public:

	void setUp() {
		core_init();

	}

	void tearDown() {

	}


	void test_mover_factory() {
		protocols::moves::MoverOP base_mover_op = protocols::moves::MoverFactory::get_instance()->newMover("BootCampMover");
		protocols::bootcamp::BootCampMoverOP bcm_op = utility::pointer::dynamic_pointer_cast<protocols::bootcamp::BootCampMover > (base_mover_op);
		TS_ASSERT(bcm_op != 0);
		//std::cout<<bcm_op<<std::endl;
	}

};
