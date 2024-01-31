// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file   test/protocols/bootcamp/FoldTreeFromSS.cxxtest.hh
/// @brief
/// @author Elijah Day (elijahday2021@gmail.com)


// Test headers
#include <cxxtest/TestSuite.h>


#include <test/util/pose_funcs.hh>
#include <test/core/init_util.hh>

// Utility headers

/// Project headers
#include <core/types.hh>

// C++ headers

// Headers for question 4
#include <core/pose/Pose.hh>
#include <core/scoring/dssp/Dssp.hh>
#include <core/kinematics/FoldTree.hh>
#include <test/util/pose_funcs.hh>
#include <protocols/bootcamp/fold_tree_from_ss.hh>
//using namespace protocols::match;
//using namespace protocols::match::upstream;


// --------------- Test Class --------------- //

class FoldTreeFromSS : public CxxTest::TestSuite {
core::pose::Pose mypose;
public:


	// --------------- Fixtures --------------- //

	// Define a test fixture (some initial state that several tests share)
	// In CxxTest, setUp()/tearDown() are executed around each test case. If you need a fixture on the test
	// suite level, i.e. something that gets constructed once before all the tests in the test suite are run,
	// suites have to be dynamically created. See CxxTest sample directory for example.


	// Shared initialization goes here.
	void setUp() {
		core_init();
		mypose = create_test_in_pdb_pose();
		
	}

	// Shared finalization goes here.
	void tearDown() {
	}

	utility::vector1< std::pair< core::Size, core::Size > > identify_secondary_structure_spans( std::string const & ss_string ) {
		utility::vector1 < std::pair< core::Size, core::Size > > ss_boundaries;
		core::Size strand_start = -1;
		for ( core::Size ii = 0; ii < ss_string.size(); ++ii) {
			if ( ss_string[ ii ] == 'E' || ss_string[ ii ] == 'H' ) {
				if ( int( strand_start ) == -1 ) {
					strand_start = ii;
				}
				else if ( ss_string[ii] != ss_string[strand_start] ) {
					ss_boundaries.push_back( std::make_pair ( strand_start+1, ii ) );
					strand_start = ii;
				}
			} 
			else {
				if ( int( strand_start ) != -1 ) {
					ss_boundaries.push_back( std::make_pair( strand_start+1, ii ) );
					strand_start = -1;
				}
			}
		}
		if ( int( strand_start) != -1 ) {
			ss_boundaries.push_back( std::make_pair( strand_start+1, ss_string.size() ));
		}
		for ( core::Size ii =1; ii <= ss_boundaries.size(); ++ii ) {
			std::cout << "SS Element " << ii << " from residue " << ss_boundaries[ ii ].first << " to " << ss_boundaries[ ii ].second << std::endl;
		}
		return ss_boundaries;
	}
	
	void test_hello_world() {
		utility::vector1< std::pair< core::Size, core::Size > > test_vec = protocols::bootcamp::identify_secondary_structure_spans("   EEEEE   HHHHHHHH  EEEEE   IGNOR EEEEEE   HHHHHHHHHHH  EEEEE  HHHH    ");
		TS_ASSERT( true );
	}
	
	void test_ignore() {
		utility::vector1< std::pair< core::Size, core::Size > > test_vec = protocols::bootcamp::identify_secondary_structure_spans("   EEEEE   HHHHHHHH  EEEEE   IGNOR EEEEEE   HHHHHHHHHHH  EEEEE  HHHH    ");
		for (core::Size ii = 1; ii <= test_vec.size(); ++ii ){
			TS_ASSERT( test_vec [ ii ].first < test_vec [ ii ].second);
		}
	}

	void test_second() {
		utility::vector1< std::pair< core::Size, core::Size > > test_vec = protocols::bootcamp::identify_secondary_structure_spans("HHHHHHH   HHHHHHHHHHHH      HHHHHHHHHHHHEEEEEEEEEEHHHHHHH EEEEHHH ");
		for (core::Size ii = 1; ii < test_vec.size(); ++ii ){
			TS_ASSERT( test_vec [ ii ].first < test_vec [ ii ].second);
		}
	}
	
	void test_third() {
		utility::vector1< std::pair< core::Size, core::Size > > test_vec = protocols::bootcamp::identify_secondary_structure_spans("EEEEEEEEE EEEEEEEE EEEEEEEEE H EEEEE H H H EEEEEEEE");
		for (core::Size ii = 1; ii < test_vec.size(); ++ii ){
			TS_ASSERT( test_vec [ ii ].first <= test_vec [ ii ].second);
		}
	}

	core::kinematics::FoldTree fold_tree_from_ss(core::pose::Pose const & pose) {
		core::scoring::dssp::Dssp dssp( pose );
		std::string ss = dssp.get_dssp_secstruct();
		core::kinematics::FoldTree ft_ss = protocols::bootcamp::fold_tree_from_dssp_string(ss);
		return ft_ss;
	}

	core::kinematics::FoldTree fold_tree_from_dssp_string(std::string const & ss) {
		core::kinematics::FoldTree ft;
		utility::vector1< std::pair< core::Size, core::Size > > ss_vec = protocols::bootcamp::identify_secondary_structure_spans(ss);
		utility::vector1< std::pair< core::Size, core::Size> > gap_vec;
		if (ss_vec[1].first != 1) {
			gap_vec.push_back( std::make_pair( 1, ss_vec[1].first-1));
			for (core::Size ii = 2; ii <= ss_vec.size(); ++ii) {
				gap_vec.push_back( std::make_pair(ss_vec[ii-1].second+1,ss_vec[ii].first-1));
			}
		}
		else {
			gap_vec.push_back( std::make_pair( ss_vec[1].second+1, ss_vec[2].first-1));
			for (core::Size ii = 3; ii <= ss_vec.size(); ++ii) {
				gap_vec.push_back( std::make_pair(ss_vec[ii-1].second+1,ss_vec[ii].first-1));
            }
		}
		//std::cout << gap_vec[1].first << std::endl;
		//std::cout << gap_vec[1].second << std::endl;
		
		//for (core::Size ii = 2; ii <= ss_vec.size(); ++ii) {
		//	gap_vec.push_back( std::make_pair(ss_vec[ii-1].second+1,ss_vec[ii].first-1));
		//}
		
		//std::cout << "Size: " << ss_vec[ss_vec.size()].second << std::endl;
		//std::cout << "String Size: " << ss.size() << std::endl;
		if (ss_vec[ss_vec.size()].second < ss.size()) {
			gap_vec.push_back( std::make_pair(ss_vec[ss_vec.size()].second+1,ss.size()));
		}

		
		core::Size jumpstart = (ss_vec[1].second + ss_vec[1].first) / 2;
		core::Size jump_counter = 1;
		if (gap_vec[1].first == 1) { //Adds first edges
			ft.add_edge(jumpstart, 1, core::kinematics::Edge::PEPTIDE );
			ft.add_edge(jumpstart, ss_vec[1].second, core::kinematics::Edge::PEPTIDE );
		}
		else {
			ft.add_edge(jumpstart, ss_vec[1].first, core::kinematics::Edge::PEPTIDE );
			ft.add_edge(jumpstart, ss_vec[1].second, core::kinematics::Edge::PEPTIDE );
			core::Size first_loop_mid = ((gap_vec[1].first + gap_vec[1].second) / 2);
			ft.add_edge(jumpstart, first_loop_mid, jump_counter);
			jump_counter +=1;
			std::cout << "Gap mid: " << first_loop_mid << std::endl;
			ft.add_edge(first_loop_mid, gap_vec[1].first, core::kinematics::Edge::PEPTIDE );
			ft.add_edge(first_loop_mid, gap_vec[1].second, core::kinematics::Edge::PEPTIDE );
		}
		
		//core::Size jump_counter = 1;
		
		for( core::Size ii=2; ii<=ss_vec.size(); ++ii) {
			core::Size midpoint = ((ss_vec[ ii ].first + ss_vec[ ii ].second) / 2);
			ft.add_edge(jumpstart, midpoint, jump_counter);
			jump_counter += 1;
		}

		for( core::Size ii = 2; ii < gap_vec.size(); ++ii) {
			core::Size gap_midpoint = ((gap_vec[ ii ].first + gap_vec [ ii ].second) / 2);
			ft.add_edge(jumpstart, gap_midpoint, jump_counter);
			jump_counter += 1;
		}

		for (core::Size ii = 2; ii <ss_vec.size(); ++ii) { //Adding SS edges for middle segments
			core::Size midpoint = ((ss_vec[ ii ].first + ss_vec [ ii ].second) /2);
			ft.add_edge(midpoint, ss_vec[ ii ].first, core::kinematics::Edge::PEPTIDE );
			ft.add_edge(midpoint, ss_vec[ ii ].second, core::kinematics::Edge::PEPTIDE );
		}

		for (core::Size ii = 2; ii < gap_vec.size(); ++ii) { //Adding loop edges for middle segments
			core::Size loop_midpoint = ((gap_vec[ ii ].first + gap_vec[ ii ].second) / 2);
			ft.add_edge(loop_midpoint, gap_vec[ ii ].first, core::kinematics::Edge::PEPTIDE );
			ft.add_edge(loop_midpoint, gap_vec[ ii ].second, core::kinematics::Edge::PEPTIDE );
		}

		if (ss_vec[ss_vec.size()].second < ss.size()) { // Adding loop edges for end
			core::Size midpoint = ((ss_vec[ss_vec.size()].first + ss_vec[ss_vec.size()].second) / 2);
			ft.add_edge(midpoint, ss_vec[ss_vec.size()].first, core::kinematics::Edge::PEPTIDE );
			ft.add_edge(midpoint, ss.size(), core::kinematics::Edge::PEPTIDE );
		}
		else {
			core::Size midpoint = ((ss_vec[ss_vec.size()].first + ss_vec[ss_vec.size()].second) / 2);
			ft.add_edge(midpoint, ss_vec[ss_vec.size()].first, core::kinematics::Edge::PEPTIDE );
			ft.add_edge(midpoint, ss_vec[ss_vec.size()].second, core::kinematics::Edge::PEPTIDE );
		}
			
		return ft;
	}
	
	void test_ftss() {
		std::string test_string ="EEEEEEE    EEEEEEE         EEEEEEEEE    EEEEEEEEEE   HHHHHH         EEEEEEEEE         EEEEE     ";
		core::kinematics::FoldTree ft = fold_tree_from_dssp_string(test_string);
		std::cout << ft << std::endl;
		TS_ASSERT(ft.check_fold_tree());
		//std::cout << "Check fold tree: " << ft.check_fold_tree() << std::endl;
	}

	void test_ftss_gap() {
		std::string test_string ="   EEEEEEE    EEEEEEE         EEEEEEEEE    EEEEEEEEEE   HHHHHH         EEEEEEEEE         EEEEE     ";
		core::kinematics::FoldTree ft = fold_tree_from_dssp_string(test_string);
		std::cout << ft << std::endl;
		TS_ASSERT(ft.check_fold_tree());
	}
	void test_ftss_pose() {
		core::kinematics::FoldTree ft = protocols::bootcamp::fold_tree_from_ss(mypose);
		std::cout << ft << std::endl;
		TS_ASSERT(ft.check_fold_tree());
	}
};
