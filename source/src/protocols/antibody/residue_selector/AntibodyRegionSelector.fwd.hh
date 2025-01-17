// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file protocols/antibody/residue_selector/AntibodyRegionSelector.fwd.hh
/// @brief A simple selector to select residues of particular antibody regions.
/// @author Jared Adolf-Bryfogle (jadolfbr@gmail.com)


#ifndef INCLUDED_protocols_antibody_residue_selector_AntibodyRegionSelector_fwd_hh
#define INCLUDED_protocols_antibody_residue_selector_AntibodyRegionSelector_fwd_hh

// Utility headers
#include <utility/pointer/owning_ptr.hh>



// Forward
namespace protocols {
namespace antibody {
namespace residue_selector {

class AntibodyRegionSelector;

typedef utility::pointer::shared_ptr< AntibodyRegionSelector > AntibodyRegionSelectorOP;
typedef utility::pointer::shared_ptr< AntibodyRegionSelector const > AntibodyRegionSelectorCOP;



} //protocols
} //antibody
} //residue_selector


#endif //INCLUDED_protocols_antibody_residue_selector_AntibodyRegionSelector_fwd_hh





