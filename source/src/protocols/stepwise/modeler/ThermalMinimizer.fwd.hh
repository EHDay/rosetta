// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file protocols/stepwise/modeler/ThermalMinimizer.fwd.hh
/// @brief Use a simulated tempering simulation to refine a pose
/// @author Andy Watkins (amw579@nyu.edu)


#ifndef INCLUDED_protocols_stepwise_modeler_ThermalMinimizer_fwd_hh
#define INCLUDED_protocols_stepwise_modeler_ThermalMinimizer_fwd_hh

// Utility headers
#include <utility/pointer/owning_ptr.hh>



// Forward
namespace protocols {
namespace stepwise {
namespace modeler {

class ThermalMinimizer;

typedef utility::pointer::shared_ptr< ThermalMinimizer > ThermalMinimizerOP;
typedef utility::pointer::shared_ptr< ThermalMinimizer const > ThermalMinimizerCOP;



} //protocols
} //stepwise
} //modeler


#endif //INCLUDED_protocols_stepwise_modeler_ThermalMinimizer_fwd_hh




