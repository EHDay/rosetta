// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file   protocols/denovo_design/residue_selectors/NamedSegmentSelector.hh
/// @brief  Creator for NamedSegmentSelector
/// @author Tom Linsky (tlinsky@uw.edu)

#ifndef INCLUDED_protocols_denovo_design_residue_selectors_NamedSegmentSelectorCreator_HH
#define INCLUDED_protocols_denovo_design_residue_selectors_NamedSegmentSelectorCreator_HH

// Package headers
#include <core/select/residue_selector/ResidueSelectorCreator.hh>

// Utility headers
#include <utility/tag/XMLSchemaGeneration.fwd.hh>

namespace protocols {
namespace denovo_design {
namespace residue_selectors {

class NamedSegmentSelectorCreator : public core::select::residue_selector::ResidueSelectorCreator {
public:
	core::select::residue_selector::ResidueSelectorOP
	create_residue_selector() const override;

	std::string
	keyname() const override;

	void
	provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd ) const override;
};

} //protocols
} //denovo_design
} //residue_selectors

#endif //INCLUDED_protocols/denovo_design/residue_selectors_NamedSegmentSelectorCreator.hh
