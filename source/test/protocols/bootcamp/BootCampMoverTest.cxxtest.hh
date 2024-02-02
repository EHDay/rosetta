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

#include<iostream>
// Test headers
#include <test/UMoverTest.hh>
#include <test/UTracer.hh>
#include <cxxtest/TestSuite.h>
#include <test/util/pose_funcs.hh>
#include <test/core/init_util.hh>
#include <string>
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

#include <core/scoring/ScoreFunctionFactory.hh>

#include <utility/tag/Tag.hh>
#include <basic/options/option.hh>
#include <basic/datacache/DataMap.hh>
#include <protocols/filters/Filter.hh>
#include <protocols/filters/BasicFilters.hh>
#include <test/util/rosettascripts.hh>


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

	void test_iteration_setter_getter()	{
		protocols::moves::MoverOP base_mover_op = protocols::moves::MoverFactory::get_instance()->newMover("BootCampMover");
        protocols::bootcamp::BootCampMoverOP bcm_op = utility::pointer::dynamic_pointer_cast<protocols::bootcamp::BootCampMover > (base_mover_op);
		core::Size iterations = 10;
		bcm_op->set_iterations(iterations);
		//bcm_op.set_iterations(iterations);
		core::Size bcm_iter = bcm_op->get_iteration();
		TS_ASSERT(iterations == bcm_iter);
	}

	void test_sfxn_setter_getter() {
		protocols::moves::MoverOP base_mover_op = protocols::moves::MoverFactory::get_instance()->newMover("BootCampMover");
		protocols::bootcamp::BootCampMoverOP bcm_op = utility::pointer::dynamic_pointer_cast<protocols::bootcamp::BootCampMover > (base_mover_op);
		core::scoring::ScoreFunctionOP sfxn = bcm_op->get_sfxn();
		//std::cout << "sfxn" << sfxn << std::endl;
		core::scoring::ScoreFunctionOP sfxn2 = core::scoring::get_score_function();
		sfxn2 ->set_weight( core::scoring::linear_chainbreak, 1.0);
		bcm_op->set_sfxn(sfxn2);
		core::scoring::ScoreFunctionOP sfxn3 = bcm_op->get_sfxn();
		//std::cout << "sfxn2: " << sfxn2 << "sfxn3: " << sfxn3 << std::endl;
		TS_ASSERT(sfxn2 == sfxn3);
	}

	void test_parse_my_tag() {
		using basic::datacache::DataMap;
		protocols::moves::MoverOP base_mover_op = protocols::moves::MoverFactory::get_instance()->newMover("BootCampMover");
		protocols::bootcamp::BootCampMoverOP bcm_op = utility::pointer::dynamic_pointer_cast<protocols::bootcamp::BootCampMover > (base_mover_op);
		basic::datacache::DataMap data;
		//data.add("niterations", "iterations",1);
		core::scoring::ScoreFunctionOP sfxn = core::scoring::get_score_function();
		data.add("scorefxns", "testing123", sfxn );
		std::string xml_file = "<BootCampMover scorefxn=\"testing123\"/>";
		utility::tag::TagCOP mytag = tagptr_from_string(xml_file);
		bcm_op->parse_score_function(mytag, data);
		bcm_op->parse_my_tag(mytag,data);
		TS_ASSERT_EQUALS(sfxn, bcm_op->get_sfxn())
	}
};
