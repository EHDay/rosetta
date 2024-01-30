// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file  protocols/bootcamp/QueueTests.cxxtest.hh
/// @brief  Bootcamp lab 3 tests
/// @author Elijah Day (elijahday2021@gmail.com)


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

#include <protocols/bootcamp/Queue.hh>
#include <utility/VirtualBase.hh>
static basic::Tracer TR("QueueTests");


class QueueTests : public CxxTest::TestSuite {
	//Define Variables

public:

	void setUp() {
		protocols::bootcamp::Queue q_;
		//protocols::bootcamp::Queue q_full("First","Second");

	}

	void tearDown() {
	}
	
	void test_first() {
		TS_TRACE( "Running my first unit test!" );
		TS_ASSERT( true );
	}

	void test_enqueue() {
		q_.enqueue("Test");
		TS_ASSERT(q_.size() == 1);
		TS_TRACE(q_.size());
	}

	void test_dequeue() {
		q_.enqueue("Test2");
		q_.dequeue();
		TS_ASSERT(q_.size() == 1);
	}

	void test_size() {
		q_.enqueue("Test3");
		TS_ASSERT(q_.size() == 2);
	}

	void test_is_empty() {
		TS_ASSERT(not q_.is_empty());
	}

private:
	protocols::bootcamp::Queue q_;
	//protocols::bootcamp::Queue q_full;





};
