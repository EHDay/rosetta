// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file basic/database/schema_generator/PrimaryKey.cc
///
/// @brief PrimaryKey class for the schema generator framework
/// @author Tim Jacobs

#include <basic/database/schema_generator/PrimaryKey.hh>
#include <utility>
#include <utility/sql_database/DatabaseSessionManager.fwd.hh>
#include <basic/database/schema_generator/Column.hh>

namespace basic {
namespace database {
namespace schema_generator {

PrimaryKey::PrimaryKey()= default;

PrimaryKey::PrimaryKey(Column column){
	columns_.push_back(column);
}

PrimaryKey::PrimaryKey(Columns columns):
	columns_(std::move( columns ))
{}

void PrimaryKey::add_column(Column column){
	columns_.push_back(column);
}

Columns const &
PrimaryKey::columns() const {
	return columns_;
}

std::string
PrimaryKey::print(
	utility::sql_database::sessionOP /*db_session*/
) const {
	std::string primary_key_string = "PRIMARY KEY (";

	for ( auto it=columns_.begin(); it != columns_.end(); ++it ) {
		if ( it!=columns_.begin() ) {
			primary_key_string += ", ";
		}
		primary_key_string += it->name();
	}

	primary_key_string += ")";
	return primary_key_string;
}

} // schema_generator
} // namespace database
} // namespace utility

