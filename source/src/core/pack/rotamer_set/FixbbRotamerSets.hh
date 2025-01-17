// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file   core/pack/RotamerSet/FixbbRotamerSets.hh
/// @brief  Fixed-backbone Residue Sets interface class declaration
/// @author Andrew Leaver-Fay (leaverfa@email.unc.edu)


#ifndef INCLUDED_core_pack_rotamer_set_FixbbRotamerSets_hh
#define INCLUDED_core_pack_rotamer_set_FixbbRotamerSets_hh

// Unit Headers
#include <core/pack/rotamer_set/FixbbRotamerSets.fwd.hh>

// Package Headers

#include <core/pack/rotamer_set/RotamerSet.fwd.hh>
#include <core/pack_basic/RotamerSetsBase.hh>


// Project Headers

// Utility Headers
#include <utility/vector1.hh>
#include <core/types.hh>

#ifdef WIN32
#include <core/pack/rotamer_set/RotamerSet.hh>
#endif

#ifdef    SERIALIZATION
// Cereal headers
#include <cereal/types/polymorphic.fwd.hpp>
#endif // SERIALIZATION

namespace core {
namespace pack {
namespace rotamer_set {


class FixbbRotamerSets : public pack_basic::RotamerSetsBase
{
public:
	typedef pack_basic::RotamerSetsBase parent;
	typedef utility::vector1< RotamerSetOP > RotamerSetVector;

public:
	FixbbRotamerSets();
	~FixbbRotamerSets() override;

	// we want to make sure that this class in bound in PyRosetta so PyRosetts `knows` that core::pack::rotamer_set::RotamerSets is descendant of core::pack_basic::RotamerSetsBase
	// and for that we need all intermediate base classes to be bindable in Python, however classes with pure-virtual version of both const and non-const functions is not bindable in Python
	// so we have to change class definition slightly for PyRosetta and declared `const` version of function with default implementation
	// Note that we do not expect `FixbbRotamerSets` to be derived-from in Python - we just need it to be bound in Python
#ifdef PYROSETTA
	virtual
	RotamerSetCOP
	rotamer_set_for_residue( uint resid ) const { return RotamerSetCOP(); }
#else
	virtual
	RotamerSetCOP
	rotamer_set_for_residue( uint resid ) const = 0;
#endif

	virtual
	RotamerSetOP
	rotamer_set_for_residue( uint resid ) = 0;


#ifdef PYROSETTA
	virtual
	RotamerSetCOP
	rotamer_set_for_moltenresidue( uint moltenresid ) const { return RotamerSetCOP(); }
#else
	virtual
	RotamerSetCOP
	rotamer_set_for_moltenresidue( uint moltenresid ) const = 0;
#endif

	virtual
	RotamerSetOP
	rotamer_set_for_moltenresidue( uint moltenresid ) = 0;


	virtual
	RotamerSetVector::const_iterator begin() = 0;

	virtual
	RotamerSetVector::const_iterator end() = 0;

	virtual
	utility::vector1< uint > const &
	resid_2_moltenres_vector() const = 0;

	virtual
	utility::vector1< uint > const &
	moltenres_2_resid_vector() const = 0;

	void
	show( std::ostream & out ) const override = 0;
#ifdef    SERIALIZATION
public:
	template< class Archive > void save( Archive & arc ) const;
	template< class Archive > void load( Archive & arc );
#endif // SERIALIZATION

};

} // namespace rotamer_set
} // namespace pack
} // namespace core


#ifdef    SERIALIZATION
CEREAL_FORCE_DYNAMIC_INIT( core_pack_rotamer_set_FixbbRotamerSets )
#endif // SERIALIZATION


#endif // INCLUDED_core_pack_RotamerSet_RotamerSets_HH
