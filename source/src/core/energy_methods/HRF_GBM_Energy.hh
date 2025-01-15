// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file   core/energy_methods/HRF_GBM_Energy.hh
/// @brief  Energy term used for scoring HRF_GBM_Energy
/// @author Sarah Biehn

#ifndef INCLUDED_core_energy_methods_HRF_GBM_Energy_hh
#define INCLUDED_core_energy_methods_HRF_GBM_Energy_hh

#include <core/scoring/methods/ContextDependentOneBodyEnergy.hh>
#include <core/scoring/ScoreFunction.fwd.hh>
#include <core/pose/Pose.fwd.hh>
#include <utility/vector1.hh>

#include <core/scoring/methods/EnergyMethodOptions.fwd.hh> // AUTO IWYU For EnergyMethodOptions


namespace core {
namespace energy_methods {



class HRF_GBM_Energy : public core::scoring::methods::ContextDependentOneBodyEnergy {
public:

	typedef core::scoring::methods::ContextDependentOneBodyEnergy parent;
	HRF_GBM_Energy( core::scoring::methods::EnergyMethodOptions const & options );

	/// clone
	core::scoring::methods::EnergyMethodOP
	clone() const override;

	/////////////////////////////////////////////////////////////////////////////
	// scoring
	/////////////////////////////////////////////////////////////////////////////

	void
	residue_energy(
		conformation::Residue const & rsd,
		pose::Pose const & pose,
		core::scoring::EnergyMap & emap
	) const override;

	void
	setup_for_scoring(
		pose::Pose & pose, core::scoring::ScoreFunction const &
	) const override;

	void
	finalize_total_energy(
		pose::Pose &,
		core::scoring::ScoreFunction const &,
		core::scoring::EnergyMap &
	) const override {}

	core::Size version() const override;

	void
	indicate_required_context_graphs(
		utility::vector1< bool > & /*context_graphs_required*/
	) const override;

private:
	std::string hrf_gbm_input_file_;
	void init_from_file();
	utility::vector1< std::pair< core::Size, core::Real > > input_nc_;
};

}
}

#endif // INCLUDED_core_energy_methods_HRF_GBM_Energy_HH
