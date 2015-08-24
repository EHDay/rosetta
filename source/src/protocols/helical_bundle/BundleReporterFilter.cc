// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington UW TechTransfer, email: license@u.washington.edu.

/// @file protocols/helical_bundle/BundleReporterFilter.cc
/// @brief A filter that is mainly intended for reporting helical bundle parameters and an associated energy.
/// @author Vikram K. Mulligan (vmullig@uw.edu)


//Unit Headers
#include <protocols/helical_bundle/BundleReporterFilter.hh>
#include <protocols/helical_bundle/BundleReporterFilterCreator.hh>

//Project Headers
#include <basic/Tracer.hh>
#include <core/scoring/ScoreFunction.hh>
#include <core/scoring/ScoreFunctionFactory.hh>
#include <core/scoring/ScoreTypeManager.hh>
#include <core/scoring/ScoreType.hh>
#include <core/scoring/Energies.hh>
#include <core/pose/Pose.hh>
#include <utility/tag/Tag.hh>
#include <basic/datacache/DataMap.hh>
#include <utility/exit.hh>
#include <ObjexxFCL/FArray1D.hh>
#include <ObjexxFCL/FArray1D.fwd.hh>
#include <ObjexxFCL/format.hh>
#include <core/util/SwitchResidueTypeSet.hh>
#include <core/chemical/AtomType.hh>
#include <core/chemical/ChemicalManager.fwd.hh>
#include <protocols/helical_bundle/parameters/BundleParametersSet.hh>
#include <protocols/helical_bundle/parameters/BundleParameters.hh>

//JD2:
#include <protocols/jd2/JobDistributor.hh>
#include <protocols/jd2/Job.hh>
#include <protocols/jd2/util.hh>

//Auto Headers
#include <utility/excn/Exceptions.hh>
#include <protocols/elscripts/util.hh>
#include <protocols/rosetta_scripts/util.hh>
#include <basic/options/keys/score.OptionKeys.gen.hh>
#include <basic/options/option.hh>
#include <basic/options/keys/out.OptionKeys.gen.hh> // for option[ out::file::silent  ] and etc.
#include <basic/options/keys/in.OptionKeys.gen.hh> // for option[ in::file::tags ] and etc.
#include <basic/options/keys/OptionKeys.hh>
#include <basic/options/option_macros.hh>
#include <core/pose/util.hh>

namespace protocols {
namespace helical_bundle {

using namespace core;
using namespace core::scoring;
using namespace ObjexxFCL::format;

/// @brief Constructor
///
BundleReporterFilter::BundleReporterFilter() :
	Filter( "BundleReporter" ),
	score_type_threshold_(0.0),
	score_type_( core::scoring::score_type_from_name( "total_score" ) ),
	scorefxn_(), //Null pointer by default.
	behaviour_(brf_always_true),
	report_sequence_(false),
	report_three_letter_codes_(false)
{}

/// @brief Copy constructor
///
BundleReporterFilter::BundleReporterFilter( BundleReporterFilter const &src ) :
	Filter( "BundleReporter" ),
	score_type_threshold_(src.score_type_threshold_),
	score_type_( src.score_type_ ),
	scorefxn_( src.scorefxn_->clone() ), //CLONE the scorefunction, don't copy it.
	behaviour_(src.behaviour_),
	report_sequence_(src.report_sequence_),
	report_three_letter_codes_(src.report_three_letter_codes_)
{}


static thread_local basic::Tracer TR( "protocols.helical_bundle.BundleReporterFilter" );
static thread_local basic::Tracer TRReport( "protocols.helical_bundle.BundleReporterFilter.REPORT" );

protocols::filters::FilterOP
BundleReporterFilterCreator::create_filter() const { return protocols::filters::FilterOP( new BundleReporterFilter ); }

std::string
BundleReporterFilterCreator::keyname() const { return "BundleReporter"; }

/// @brief Destructor.
///
BundleReporterFilter::~BundleReporterFilter() {}

/// @brief Parse XML (RosettaScripts) setup.
///
void
BundleReporterFilter::parse_my_tag( utility::tag::TagCOP tag, basic::datacache::DataMap & data, filters::Filters_map const &, moves::Movers_map const &, core::pose::Pose const & )
{
	using namespace core::scoring;

	// Parse scorefunction:
	{
		runtime_assert_string_msg( tag->hasOption("scorefxn"), "Error in protocols::helical_bundle::BundleReporterFilter::parse_my_tag():  The BundleReporter filter must have a scorefunction specified (\"scorefxn\" option)." );
		set_scorefxn( protocols::rosetta_scripts::parse_score_function( tag, data )->clone() );
		if ( TR.visible() ) TR << "Set scorefunction for the BundleReporter filter to " << tag->getOption<std::string>("scorefxn") << "." << std::endl;
	}

	// Parse scoretype, if specified.  (Default to "total_score" if not.):
	{
		std::string const scoretype_string( tag->getOption<std::string>( "score_type", "total_score" ) );
		set_score_type( core::scoring::score_type_from_name( scoretype_string ) );
		if ( TR.visible() ) TR << "Set BundleReporter filter scoretype to " << scoretype_string << "." << std::endl;
	}

	// Parse behaviour, if specified
	{
		std::string const behav_string( tag->getOption< std::string >( "behavior", "ALWAYS_TRUE" ) ); //Ugh -- user-facing side uses American spelling of "behaviour".
		set_filter_behaviour( behav_string );
		if ( TR.visible() ) TR << "Set BundleReporter filter behaviour to " << behav_string << "." << std::endl;
	}

	// Parse energy threshold, if specified and used:
	if ( tag->hasOption( "threshold" ) ) {
		if ( filter_behaviour() != brf_filter_by_energy && TR.Warning.visible() ) {
			if ( TR.visible() ) TR.flush();
			TR.Warning << "Warning!  A \"threshold\" option was specified in the setup for the BundleReporterFilter, but this will be ignored because the filter behaviour was not set to \"FILTER\"." << std::endl;
			TR.Warning.flush();
		}
	}
	if ( filter_behaviour() == brf_filter_by_energy ) {
		core::Real const thresh( tag->getOption<core::Real>( "threshold", 0.0 ) );
		set_score_type_threshold( thresh );
		if ( TR.visible() ) TR << "Energy threshold for filtering set to " << thresh << ".  (Energies above this will result in the filter returning FALSE)." << std::endl;
	}

	// Parse options for reporting sequences:
	set_report_sequence( tag->getOption<bool>("report_sequence", false) );
	if ( tag->hasOption( "use_three_letter_code" ) && !report_sequence() && TR.Warning.visible() ) {
		if ( TR.visible() ) TR.flush();
		TR.Warning << "Warning!  The \"use_three_letter_code\" option was specified, but it will be ignored since \"report_sequence\" is set to false." << std::endl;
		TR.Warning.flush();
	}
	set_use_threeletter( tag->getOption<bool>("use_three_letter_code", false) );
	if ( report_sequence() && TR.visible() ) {
		if ( use_threeletter() ) {
			TR << "Sequences will be reported by the BundleReporterFilter as three-letter codes." << std::endl;
		} else {
			TR << "Sequences will be reported by the BundleReporterFilter as one-letter codes." << std::endl;
		}
	}
	// Flush all tracers:
	if ( TR.visible() ) TR.flush();
	if ( TR.Warning.visible() ) TR.Warning.flush();
	if ( TR.Debug.visible() ) TR.Debug.flush();

	return;
}


/// @brief Set the behaviour by string.
/// @details Options are "ALWAYS_TRUE", "ALWAYS_FALSE", or "FILTER".
void BundleReporterFilter::set_filter_behaviour( std::string const &behaviour_string )
{
	if ( behaviour_string == "ALWAYS_TRUE" ) set_filter_behaviour( brf_always_true );
	else if ( behaviour_string == "ALWAYS_FALSE" ) set_filter_behaviour( brf_always_false );
	else if ( behaviour_string == "FILTER" ) set_filter_behaviour( brf_filter_by_energy );
	else set_filter_behaviour( brf_undefined ); //Will throw an error.

	return;
}


/// @brief Actually apply the filter to a pose.
/// @details This scores the pose with the scorefunction, writes out the energy and the
/// bundle parameters (if any) to the REPORT tracer, then applies the selected behaviour
/// of the filter (always true, always false, or actually filtering by score).
bool
BundleReporterFilter::apply( core::pose::Pose const & pose ) const {
	bool returnval(true);

	/// Compute the energy of the pose:
	core::Real const score( compute( pose ) );

	/// Write out a full report:
	if ( TRReport.visible() ) {
		core::Size jobno(0);
		if ( protocols::jd2::jd2_used() ) { //If this is a JD2 job
			jobno=protocols::jd2::JobDistributor::get_instance()->current_job()->nstruct_index(); //Get the current job number.
		}
		TRReport << generate_full_tracer_report( jobno, score, pose ) << std::endl;
		TRReport.flush();
	}

	if ( filter_behaviour() == brf_filter_by_energy ) {
		if (  score <= score_type_threshold() ) {
			if ( TR.visible() ) TR << "Energy is less than threshold.  Passing." << std::endl;
			returnval=true;
		} else {
			if ( TR.visible() ) TR << "Energy is greater than threshold.  Failing." << std::endl;
			returnval=false;
		}
	} else if ( filter_behaviour() == brf_always_false ) {
		returnval=false;
	} else if ( filter_behaviour() == brf_always_true ) {
		returnval=true;
	}

	// Flush all tracers:
	if ( TR.visible() ) TR.flush();
	if ( TR.Warning.visible() ) TR.Warning.flush();
	if ( TR.Debug.visible() ) TR.Debug.flush();

	return returnval;
}

/// @brief Allows reporting of filter values to a stream.
///
void
BundleReporterFilter::report( std::ostream & out, core::pose::Pose const & pose ) const {
	out << "Weighted score of "<<core::scoring::ScoreTypeManager::name_from_score_type( score_type_ )<<" "<<compute( pose )<<'\n';
}

/// @brief Allows reporting of the filter value to a float.
///
core::Real
BundleReporterFilter::report_sm( core::pose::Pose const & pose ) const {
	return( compute( pose ) );
}

/// @brief Generates the text of the full report that the filter writes out to the REPORT tracer.
/// @details Called by the APPLY function.
std::string BundleReporterFilter::generate_full_tracer_report(
	core::Size const jobno,
	core::Real const &score,
	core::pose::Pose const &pose
) const {
	using namespace protocols::helical_bundle::parameters;

	std::stringstream ss;

	// Blank line first
	ss << std::endl;

	// Asterisks and the job number at the top
	if ( jobno!=0 ) {
		ss << "********** BUNDLEREPORTER FILTER REPORT FOR JOB " << jobno << " **********" << std::endl;
	} else {
		ss << "********** BUNDLEREPORTER FILTER REPORT **********" << std::endl;
	}

	ss << "Score:\t" << score << std::endl;

	/// Write out the sequence:
	if ( report_sequence() ) {
		ss << "Sequence:\t";
		if ( use_threeletter() ) {
			for ( core::Size ir=1, irmax=pose.n_residue(); ir<=irmax; ++ir ) {
				ss << pose.residue(ir).name3();
				if ( ir<irmax ) ss << " ";
				else ss << std::endl;
			}
		} else {
			ss << pose.sequence() << std::endl;
		}
	}

	core::Size const nsets(pose.conformation().n_parameters_sets()); //How many ParametersSet objects are there in the pose?
	core::Size bundleset_counter(0);

	// Get the PDB remark info and put it in the output stringstream:
	if ( nsets==0 ) {
		ss << "This is not a parametric pose!" << std::endl;
	} else {
		for ( core::Size iset=1; iset<=nsets; ++iset ) { //Loop through all of the ParametersSet objects.
			BundleParametersSetCOP curset( utility::pointer::dynamic_pointer_cast< const BundleParametersSet >( pose.conformation().parameters_set(iset) ) );
			if ( !curset ) continue; //Not a BundleParametersSet (i.e. some other sort of parametric dataset), so go on to the next.
			++bundleset_counter;
			curset->get_pdb_remark( ss );
		}
	}
	if ( bundleset_counter==0 ) {
		ss << "This parametric pose has no BundleParameterSets in it!" << std::endl;
	}

	// Asterisks and the job number at the bottom
	if ( jobno!=0 ) {
		ss << "******** END BUNDLEREPORTER FILTER REPORT FOR JOB " << jobno << " ********" << std::endl;
	} else {
		ss << "******** END BUNDLEREPORTER FILTER REPORT ********" << std::endl;
	}

	return ss.str();
}


/// @brief Computes the energy of the pose.
/// @details the energy function must be suitable for residue type set of the pose,
/// and must be symmetric if this is a symmetric pose.
core::Real
BundleReporterFilter::compute( core::pose::Pose const & pose ) const {
	using namespace core::pose;
	using namespace core::scoring;

	PoseOP in_pose( new Pose( pose ) );

	(*scorefxn_)( *in_pose );

	core::Real const weight( (*scorefxn_)[ ScoreType( score_type_ ) ] );
	core::Real const score( in_pose->energies().total_energies()[ ScoreType( score_type_ ) ]);

	if ( score_type_ == total_score ) return( score );

	core::Real const weighted_score( weight * score );
	return( weighted_score );
}

}
}