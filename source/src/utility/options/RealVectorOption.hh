// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file   utility/options/RealVectorOption.hh
/// @brief  Program real vector option class
/// @author Stuart G. Mentzer (Stuart_Mentzer@objexx.com)


#ifndef INCLUDED_utility_options_RealVectorOption_hh
#define INCLUDED_utility_options_RealVectorOption_hh


// Unit headers
#include <utility/options/RealVectorOption.fwd.hh>

// Package headers
#include <utility/options/VectorOption_T_.hh>
#include <utility/options/keys/RealVectorOptionKey.hh>

// ObjexxFCL headers
#include <ObjexxFCL/string.functions.hh>

// C++ headers
#include <cstdlib>
#include <iosfwd>


#ifdef    SERIALIZATION
// Cereal headers
#include <cereal/types/polymorphic.fwd.hpp>
#endif // SERIALIZATION

namespace utility {
namespace options {


/// @brief Program real vector option class
class RealVectorOption :
	public VectorOption_T_< RealVectorOptionKey, double >
{


private: // Types


	typedef  VectorOption_T_< RealVectorOptionKey, double >  Super;


public: // Creation


	/// @brief Default constructor
	inline
	RealVectorOption()
	{}


	/// @brief Key + description constructor
	inline
	RealVectorOption(
		RealVectorOptionKey const & key_a,
		std::string const & description_a
	) :
		Super( key_a, description_a )
	{}


	/// @brief Clone this
	inline
	RealVectorOption *
	clone() const override
	{
		return new RealVectorOption( *this );
	}


	/// @brief Destructor
	inline

	~RealVectorOption() override {}


public: // Properties


	/// @brief Is a string readable as this option's value type?
	inline
	bool
	is_value( std::string const & value_str ) const override
	{
		return ObjexxFCL::is_double( value_str );
	}


	/// @brief Is a string readable as this option's value type and a legal command line value?
	inline
	bool
	is_cl_value( std::string const & value_str ) const override
	{
		return is_value( value_str );
	}


	/// @brief Option type code string representation
	inline
	std::string
	type_string() const override
	{
		return "(R" + size_constraint_string() + ')';
	}


protected: // Methods


	/// @brief Value of a string
	inline
	Value
	value_of( std::string const & value_str ) const override
	{
		if ( ( value_str.empty() ) || ( ! ObjexxFCL::is_double( value_str ) ) ) {
			print_error_message( "Illegal value for real option -" + id()
				+ " specified: " + value_str );
			std::exit( EXIT_FAILURE );
		}
		return ObjexxFCL::double_of( value_str );
	}


	/// @brief Setup stream state for the Option value type

	void
	stream_setup( std::ostream & stream ) const override
	{
		stream << std::setprecision( 15 );
	}


#ifdef    SERIALIZATION
public:
	template< class Archive > void save( Archive & arc ) const;
	template< class Archive > void load( Archive & arc );
#endif // SERIALIZATION

}; // RealVectorOption


} // namespace options
} // namespace utility


#ifdef    SERIALIZATION
CEREAL_FORCE_DYNAMIC_INIT( utility_options_RealVectorOption )
#endif // SERIALIZATION


#endif // INCLUDED_utility_options_RealVectorOption_HH
