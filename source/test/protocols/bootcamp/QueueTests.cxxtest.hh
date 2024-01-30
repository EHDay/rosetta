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
	utility::pointer::shared_ptr< protocols::bootcamp::Queue > q_single = protocols::bootcamp::QueueOP (new protocols::bootcamp::Queue());
	utility::pointer::shared_ptr< protocols::bootcamp::Queue > q_empty = protocols::bootcamp::QueueOP (new protocols::bootcamp::Queue());
	utility::pointer::shared_ptr< protocols::bootcamp::Queue > q_multi = protocols::bootcamp::QueueOP (new protocols::bootcamp::Queue());
public:

	void setUp() {
		//q4 = protocols::bootcamp::QueueOP (new protocols::bootcamp::Queue()); 
		//protocols::bootcamp::Queue q_empty;
		//protocols::bootcamp::Queue q_single;
		//q_single = protocols::bootcamp::Queue();
		//q_single->enqueue("Test1");
		//TS_TRACE(q_single.size());
		//protocols::bootcamp::Queue q_multi;
		//q_multi = protocols::bootcamp::Queue();
		//q_multi->enqueue("Test1");
		//q_multi->enqueue("Test2");	
	}

	void tearDown() {
	}
	
	void test_first() {
		//TS_TRACE( "Running my first unit test!" );
		TS_ASSERT( true );
	}

	void test_enqueue() {
		TS_ASSERT(q_empty->is_empty())
		q_empty->enqueue("Test");
		TS_ASSERT(not q_empty->is_empty());
		TS_ASSERT(q_empty->size() == 1);
		q_empty->enqueue("Test2");
		TS_ASSERT(q_empty->size() == 2);
	}

	void test_dequeue() {
		q_single->enqueue("Test1");
		//TS_TRACE(q_single.size());
		q_single->enqueue("Test2");
		//TS_TRACE(q_single.size());
		q_single->dequeue();
		//TS_TRACE(q_single.size());
		TS_ASSERT(q_single->size() == 1);
	}

	void test_size() {
		q_multi->enqueue("Test1");
		q_multi->enqueue("Test2");
		TS_TRACE(q_multi->size());
		q_multi->enqueue("Test3");
		TS_TRACE(q_multi->size());
		TS_ASSERT(q_multi->size() == 3);
	}

	void test_is_empty() {
		q_single->dequeue();
		TS_ASSERT(q_single->is_empty());
	}

private:
	//protocols::bootcamp::QueueOP q_empty;
	//protocols::bootcamp::QueueOP q_single;
	//protocols::bootcamp::QueueOP q_multi;
	protocols::bootcamp::QueueOP  q4;
};
