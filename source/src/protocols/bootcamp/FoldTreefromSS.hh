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

namespace protocols {
namespace bootcamp {

utility::vector1< std::pair< core::Size, core::Size > > identify_secondary_structure_spans( std::string const & ss_string );


core::kinematics::FoldTree fold_tree_from_dssp_string(std::string const & ss);

class FoldTreeFromSS {
public:
	FoldTreeFromSS ( std::string const  ssstring );

	core::kinematics::FoldTree const &
	fold_tree() const;

	protocols::loops::Loop const &
	loop( core::Size index ) const;

	core::Size
	loop_for_residue( core::Size seqpos ) const;

private
	core::kinematics::Foldtree ft_;
	utility::vector1< protocols::loops::Loop < loop_vector_;
	utility::vector1< core::Size > loop_for_residue_;
};

}
}
