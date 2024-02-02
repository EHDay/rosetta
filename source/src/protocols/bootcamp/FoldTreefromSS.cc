// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file   src/protocols/bootcamp/FoldTreeFromSS.cxxtest.hh
/// @brief
/// @author Elijah Day (elijahday2021@gmail.com)





#include <core/types.hh>
#include <core/pose/Pose.hh>
#include <core/scoring/dssp/Dssp.hh>
#include <core/kinematics/FoldTree.hh>
#include <protocols/bootcamp/fold_tree_from_ss.hh>
#include <basic/Tracer.hh>
static basic::Tracer TR("Debug");
namespace protocols {
namespace bootcamp {

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
			//std::cout << "SS Element " << ii << " from residue " << ss_boundaries[ ii ].first << " to " << ss_boundaries[ ii ].second << std::endl;
	}
	return ss_boundaries;
}

FoldTreeFromSS::FoldTreeFromSS ( std::string const & ss_string) {
	core::scoring::dssp::Dssp dssp( pose );
	std::string ss = dssp.get_dssp_secstruct();
	core::kinematics::FoldTree ft_ss = fold_tree_from_dssp_string(ss);
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
            ft.add_edge(first_loop_mid, gap_vec[1].first, core::kinematics::Edge::PEPTIDE );
            ft.add_edge(first_loop_mid, gap_vec[1].second, core::kinematics::Edge::PEPTIDE );
        }


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



}
}
