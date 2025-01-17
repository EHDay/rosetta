// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file   protocols/simple_moves/sidechain_moves/SetChiMover.hh
/// @brief  A mover to change one chi angle
/// @author Noah Ollikanen

#ifndef INCLUDED_protocols_simple_moves_sidechain_moves_SetChiMover_hh
#define INCLUDED_protocols_simple_moves_sidechain_moves_SetChiMover_hh

#include <protocols/simple_moves/sidechain_moves/SetChiMover.fwd.hh>
#include <protocols/moves/Mover.hh>

// Project Headers
#include <core/pose/Pose.fwd.hh>
#include <core/types.hh>
#include <core/pose/ResidueIndexDescription.fwd.hh>

//parsing
#include <utility/tag/Tag.fwd.hh>
#include <basic/datacache/DataMap.fwd.hh>

namespace protocols {
namespace simple_moves {
namespace sidechain_moves {

/// @brief A mover to change one chi angle
class SetChiMover : public protocols::moves::Mover
{
private:
	typedef protocols::moves::Mover parent;
public:
	/// @brief default ctor
	SetChiMover();
	~SetChiMover() override;

	void apply( core::pose::Pose & pose ) override;
	protocols::moves::MoverOP clone() const  override{
		return (utility::pointer::make_shared< protocols::simple_moves::sidechain_moves::SetChiMover >( *this ) );
	}
	protocols::moves::MoverOP fresh_instance() const override {
		return utility::pointer::make_shared< SetChiMover >();
	}

	void parse_my_tag( utility::tag::TagCOP tag,
		basic::datacache::DataMap &
	) override;

	core::Real angle() const { return angle_; }
	core::Size resnum( core::pose::Pose const & pose ) const;
	core::Size chinum() const { return chinum_; }
	void angle( core::Real const a ){ angle_ = a; }
	void resnum( core::pose::ResidueIndexDescriptionCOP r );
	void resnum( core::Size const r );
	void chinum( core::Size const c ){ chinum_ = c; }

	std::string
	get_name() const override;

	static
	std::string
	mover_name();

	static
	void
	provide_xml_schema( utility::tag::XMLSchemaDefinition & xsd );


private:
	core::Real angle_;
	core::pose::ResidueIndexDescriptionCOP resnum_;
	core::Size chinum_;
};

} // sidechain_moves
} // simple_moves
} // protocols

#endif //INCLUDED_protocols_simple_moves_sidechain_moves_SetChiMover_HH_
