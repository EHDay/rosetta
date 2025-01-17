// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @brief   Transform a protein into the membrane
/// @author  JKLeman (julia.koehler1982@gmail.com)

// App headers
#include <devel/init.hh>

// Project Headers
#include <protocols/moves/Mover.fwd.hh>
#include <protocols/membrane/TransformIntoMembraneMover.hh>
#include <protocols/membrane/AddMembraneMover.hh>
#include <protocols/moves/MoverContainer.hh>

// Package Headers

#include <protocols/jd2/JobDistributor.hh>
#include <basic/Tracer.hh>
#include <utility/excn/Exceptions.hh>

// C++ Headers

using basic::Error;
using basic::Warning;

static basic::Tracer TR( "apps.pilot.jkleman.transform_into_membrane" );

int
main( int argc, char * argv [] ) {
	try {

		using namespace protocols::jd2;
		using namespace protocols::membrane;
		using namespace protocols::moves;

		devel::init(argc, argv);

		// create two movers and concatenate them in a sequence mover
		AddMembraneMoverOP addmem( new AddMembraneMover() );
		TransformIntoMembraneMoverOP transform( new TransformIntoMembraneMover() );
		SequenceMoverOP seq( new SequenceMover( addmem, transform ) );

		// call jobdistributor on sequence mover
		JobDistributor::get_instance()->go( seq );
	}
catch (utility::excn::Exception const & e ) {
	e.display();
	return -1;
}

}
