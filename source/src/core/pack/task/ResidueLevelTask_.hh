// -*- mode:c++;tab-width:2;indent-tabs-mode:t;show-trailing-whitespace:t;rm-trailing-spaces:t -*-
// vi: set ts=2 noet:
//
// (c) Copyright Rosetta Commons Member Institutions.
// (c) This file is part of the Rosetta software suite and is made available under license.
// (c) The Rosetta software is developed by the contributing members of the Rosetta Commons.
// (c) For more information, see http://www.rosettacommons.org. Questions about this can be
// (c) addressed to University of Washington CoMotion, email: license@uw.edu.

/// @file   core/pack/task/ResidueLevelTask_.hh
/// @brief  Implementation class for task class to describe packer's behavior header
/// Almost all of rosetta needs to use packer tasks, but very little of rosetta needs
/// to see how it behaves internally.
/// @author Andrew Leaver-Fay (leaverfa@email.unc.edu)
/// @author Steven Lewis

#ifndef INCLUDED_core_pack_task_ResidueLevelTask__hh
#define INCLUDED_core_pack_task_ResidueLevelTask__hh

// Unit Headers
#include <core/pack/task/ResidueLevelTask.hh>

// Package Headers
#include <core/pack/palette/PackerPalette.fwd.hh>
#include <core/pack/task/RotamerSampleOptions.hh>
#include <core/pack/rotamer_set/RotamerSetOperation.fwd.hh>
#include <core/pack/task/rna/RNA_ResidueLevelTask.fwd.hh>
#include <core/chemical/ResidueProperty.hh>

// Project Headers
#include <core/pose/Pose.fwd.hh>

// Utility Headers
#include <utility/vector1.hh>

// STL Headers
#include <iosfwd>

#include <core/conformation/Residue.fwd.hh> // AUTO IWYU For Residue


#ifdef    SERIALIZATION
// Cereal headers
#include <cereal/types/polymorphic.fwd.hpp>
#endif // SERIALIZATION

namespace core {
namespace pack {
namespace task {


/// @brief Residue-level task class
class ResidueLevelTask_ : public ResidueLevelTask
{
private:
	/// @brief Workaround for https://developercommunity.visualstudio.com/content/problem/1170468/ice-when-using-make-shared-via-a-using-declaration.html
	static core::pack::palette::PackerPaletteCOP MakeDefaultPackerPaletteCOP();

public:
	/// @brief constructor; requires a Residue object and a PackerPalette object.
	ResidueLevelTask_(
		conformation::Residue const & original_residue,
		core::pose::Pose const & pose,
		core::pack::palette::PackerPaletteCOP packer_palette_in = MakeDefaultPackerPaletteCOP()
	);

	ResidueLevelTask_();

	/// @brief dtor
	~ResidueLevelTask_() override;

	/// @brief returns the extra chi sampling level
	ExtraRotSample
	extrachi_sample_level(
		bool buried,
		int chi,
		chemical::ResidueType const & concrete_residue
	) const override;

	/// @brief initialize options from command line flags
	void initialize_from_command_line() override;

	/// @brief Initialize only the extra rotamer building flags from the command line;
	/// invoked by intialize_from_command_line();
	void initialize_extra_rotamer_flags_from_command_line() override;

	void initialize_from_options( utility::options::OptionCollection const & options ) override;
	void initialize_extra_rotamer_flags_from_options( utility::options::OptionCollection const & options ) override;


	/// @brief include the pre-existing rotamer while packing
	void or_include_current( bool include_current ) override;
	/// @brief is the pre-existing rotamer specifically allowed while packing?
	bool include_current() const override;

	// access to any flagged protocol-level behaviors for this residue
	void add_behavior( std::string const & behavior ) override;
	bool has_behavior( std::string const & behavior ) const override;
	bool has_behavior() const override;

	void target_type( chemical::ResidueTypeCOP type ) override;
	void target_type( chemical::AA aa ) override;
	void target_type( std::string name ) override;

	/// @brief include adducts at this residue
	void or_adducts( bool setting ) override;
	bool adducts() const override;

	/// @brief activate ex1 when passed true; do nothing otherwise
	void or_ex1( bool ex1 ) override;
	/// @brief activate ex2 when passed true; do nothing otherwise
	void or_ex2( bool ex2 ) override;
	/// @brief activate ex3 when passed true; do nothing otherwise
	void or_ex3( bool ex3 ) override;
	/// @brief activate ex4 when passed true; do nothing otherwise
	void or_ex4( bool ex4 ) override;

	/// @brief increase ex1 sample level; do nothing if not an increase
	void or_ex1_sample_level( ExtraRotSample ex1_sample_level ) override;
	/// @brief increase ex2 sample level; do nothing if not an increase
	void or_ex2_sample_level( ExtraRotSample ex2_sample_level ) override;
	/// @brief increase ex3 sample level; do nothing if not an increase
	void or_ex3_sample_level( ExtraRotSample ex3_sample_level ) override;
	/// @brief increase ex4 sample level; do nothing if not an increase
	void or_ex4_sample_level( ExtraRotSample ex4_sample_level ) override;

	/// @brief activate ex1 for aromatics when passed true; do nothing otherwise
	void or_ex1aro( bool ex1aro ) override;
	/// @brief activate ex2 for aromatics when passed true; do nothing otherwise
	void or_ex2aro( bool ex2aro_only ) override;
	/// @brief activate ex1 for exposed aromatics when passed true; do nothing otherwise
	void or_ex1aro_exposed( bool ex1aro_exposed ) override;
	/// @brief activate ex2 for exposed aromatics when passed true; do nothing otherwise
	void or_ex2aro_exposed( bool ex2aro_exposed ) override;

	/// @brief increase ex1aro sample level; do nothing if not an increase
	void or_ex1aro_sample_level( ExtraRotSample ex1aro_sample_level ) override;
	/// @brief increase ex2aro sample level; do nothing if not an increase
	void or_ex2aro_sample_level( ExtraRotSample ex2aro_only_sample_level ) override;
	/// @brief increase ex1aro_exposed sample level; do nothing if not an increase
	void or_ex1aro_exposed_sample_level( ExtraRotSample ex1aro_exposed_sample_level ) override;
	/// @brief increase ex2aro_exposed sample level; do nothing if not an increase
	void or_ex2aro_exposed_sample_level( ExtraRotSample ex2aro_exposed_sample_level ) override;

	void or_operate_on_ex1( bool operate ) override;
	void or_operate_on_ex2( bool operate ) override;
	void or_operate_on_ex3( bool operate ) override;
	void or_operate_on_ex4( bool operate ) override;

	void or_exdna_sample_level( ExtraRotSample exdna_sample_level ) override;

	void or_optimize_h( bool setting ) override;
	bool optimize_h() const override;
	void or_preserve_c_beta( bool setting ) override;
	bool preserve_c_beta() const override;
	void or_flip_HNQ( bool setting ) override;
	bool flip_HNQ() const override;
	void or_fix_his_tautomer( bool setting ) override;
	bool fix_his_tautomer() const override;

	void or_include_virtual_side_chain( bool setting ) override;
	bool include_virtual_side_chain() const override;

	/// @brief sample proton chi.
	void sample_proton_chi( bool setting ) override;

	/// @brief sample proton chi.
	bool sample_proton_chi() const override;

	bool ex1() const override;
	bool ex2() const override;
	bool ex3() const override;
	bool ex4() const override;

	ExtraRotSample ex1_sample_level() const override;
	ExtraRotSample ex2_sample_level() const override;
	ExtraRotSample ex3_sample_level() const override;
	ExtraRotSample ex4_sample_level() const override;

	bool ex1aro() const override;
	bool ex2aro() const override;
	bool ex1aro_exposed() const override;
	bool ex2aro_exposed() const override;

	ExtraRotSample ex1aro_sample_level() const override;
	ExtraRotSample ex2aro_sample_level() const override;
	ExtraRotSample ex1aro_exposed_sample_level() const override;
	ExtraRotSample ex2aro_exposed_sample_level() const override;

	ExtraRotSample exdna_sample_level() const override;

	bool operate_on_ex1() const override;
	bool operate_on_ex2() const override;
	bool operate_on_ex3() const override;
	bool operate_on_ex4() const override;

	/// @brief lower extrachi_cutoff to given value; do nothing if not a decrease
	void and_extrachi_cutoff( Size num_neighbors_to_be_called_buried ) override;

	/// @brief get function for extrachi_cutoff
	Size extrachi_cutoff() const override;

	/// @brief remove all ResidueTypes from the list of allowed residue types, preventing repacking
	void prevent_repacking() override;

	/// @brief Restrict residue types.
	/// @details This function takes a vector of base names to allow.  Anything not in this list is turned off.
	/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).
	void restrict_restypes( utility::vector1< std::string > const & basenames_to_keep ) override;

	/// @brief Disable residue types.
	/// @details This function takes a vector of base names to prohibit.  Anything in this list is turned off.
	/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).
	void disable_restypes( utility::vector1< std::string > const & basenames_to_disable ) override;

	/// @brief Given a list of residue properties, eliminate any residue type that does not have at
	/// least one of the properties in the list.
	/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).
	void restrict_to_restypes_with_at_least_one_property( utility::vector1< core::chemical::ResidueProperty > const & properties ) override;

	/// @brief Given a list of residue properties, eliminate any residue type that does not have ALL of the properties in the list.
	/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).
	void restrict_to_restypes_with_all_properties( utility::vector1< core::chemical::ResidueProperty > const & properties ) override;

	/// @brief Given a list of residue properties, eliminate any residue type that has any of the properties in the list.
	/// @author Vikram K. Mulligan (vmulligan@flatironinstitute.org).
	void disable_restypes_with_at_least_one_property( utility::vector1< core::chemical::ResidueProperty > const & properties ) override;

	/// @brief disables designing to residues not in the passed list
	void restrict_absent_canonical_aas( utility::vector1< bool > const &) override;

	/// @brief disables designing to residues not in the passed list--and specifies the resfile command that made this list
	void restrict_absent_canonical_aas( utility::vector1< bool > const & allowed_aas,
		std::string const & mode ) override;

	//@brief Same behavior restrict_absent_canonical_aas except that it always allows the native aa at a position even if it is not included in the allowed residues
	void restrict_nonnative_canonical_aas( utility::vector1< bool > const & allowed_aas) override;


	/// @brief disables designing to nucleic acid residues not in the passed list
	void restrict_absent_nas( utility::vector1< chemical::AA > const & keep_nas ) override;

	/// @brief only let this residue repack -- prevent redesign
	void restrict_to_repacking() override;

	/// @brief
	bool is_original_type( chemical::ResidueTypeCOP type ) const override;

	/// @brief
	chemical::ResidueTypeSetCOP get_original_residue_set() const override;

	/// @brief
	chemical::AA const & get_original_residue() const override;

	ResidueTypeCOPList const & allowed_residue_types() const override;
	/// @brief returns iterator to beginning of allowed residue types list (traversal only)
	ResidueTypeCOPListConstIter allowed_residue_types_begin() const override;
	/// @brief returns iterator to end of allowed residue types list (traversal only)
	ResidueTypeCOPListConstIter allowed_residue_types_end() const override;

	chemical::ResidueTypeCOP target_type() const override;

	void print_allowed_types( std::ostream & os ) const override;

	/// @brief is this residue up for design (variable sequence)?
	bool being_designed() const override;
	/// @brief is this residue modififable at all by the packer?
	bool being_packed() const override;

	rotamer_set::RotamerOperations const &
	rotamer_operations() const override;

	void
	append_rotamer_operation(
		rotamer_set::RotamerOperationOP rotop
	) override;

	rotamer_set::RotSetOperationListIterator
	rotamer_set_operation_begin() const override;

	rotamer_set::RotSetOperationListIterator
	rotamer_set_operation_end() const override;

	void
	append_rotamerset_operation(
		rotamer_set::RotamerSetOperationOP rotsetop
	) override;


	/// @brief create a string the resfile format of all the commands applied to this residue level task
	std::string command_string( ) const override;


	///////////////////////////// dangerous update functions

	virtual
	void
	update_union(
		ResidueLevelTask const & res_task_in
	);

	virtual
	void
	update_intersection(
		ResidueLevelTask const & res_task_in
	);

	virtual
	void
	update_commutative(
		ResidueLevelTask const & res_task_in
	);


private: // private methods

	/// @brief private: bookkeeping for ex1
	void refresh_ex1_sample_levels();
	/// @brief private: bookkeeping for ex2
	void refresh_ex2_sample_levels();
	/// @brief private: bookkeeping for ex3
	void refresh_ex3_sample_levels();
	/// @brief private: bookkeeping for ex4
	void refresh_ex4_sample_levels();

	/// @brief private: bookkeeping for whether sequence can change
	void determine_if_designing();
	/// @brief private: bookkeeping for whether rotamer can change
	void determine_if_repacking();


	/// @brief private: return the EX command for the packer task
	std::string
	get_ex_flags( Size chiid,
		Size const exaro_sample_level,
		Size const ex_sample_level) const;

	/// @brief private: return the task mode that can be used to recreate
	///the task.  If the residue level task was made with POLAR it
	///should return the string "POLAR".
	std::string task_mode() const;

	rna::RNA_ResidueLevelTask const & rna_task() const override;

	rna::RNA_ResidueLevelTask & nonconst_rna_task() override;

	void
	do_restrict_absent_canonical_aas( utility::vector1<bool> const & allowed_aas );

private:

	/// @details is the pre-existing rotamer included for the packer to choose?
	bool include_current_ = false;
	/// here we store any flagged protocol-level behaviors for this residue
	utility::vector1< std::string > behaviors_;
	/// @brief include adducts at this residue
	bool adducts_ = true;

	/// @brief What ResidueTypeSet does this set of allowed residues come from?
	chemical::ResidueTypeSetCOP original_residue_type_set_;

	/// @details std::list of ResidueTypeCOP objects - these are only residue types allowed at position
	ResidueTypeCOPList allowed_residue_types_;
	chemical::ResidueTypeCOP original_residue_type_; //record this on construction
	/// @details a member of the allowed types that can optionally respresent a target state
	chemical::ResidueTypeCOP target_residue_type_ = nullptr;

	/// @details can the sequence change?  true implies repacking_ is true as well
	bool designing_;
	/// @details can the residue change its conformation?
	bool repacking_ = true;

	/// @details are we keeping the coordinates of the heavy atoms fixed while
	///sampling extra hydrogen placements?
	bool optimize_H_mode_ = false;

	/// @details are c-beta positions preserved during rotamer building
	bool preserve_c_beta_ = false;

	/// @details are we also considering heavy atom rearrangement for the crystallographically
	///symmetric seeming residues: histadine, asparagine and glutamine? Implies optimize_H_mode_
	bool flip_HNQ_ = false;

	/// @details has this histidine tautomer been fixed?  This value is kept for bookkeeping; the tautomer's fixation is effected by removing the other tautomer from the ResidueTypeCOPList.
	bool fix_his_tautomer_ = false;

	bool include_virtual_side_chain_ = false;

	/// @details if this is true, this residue will be treated as part of the background.
	///a disabling takes precedence over any ResidueType additions.
	bool disabled_ = false;
	/// @details if this is true, this residue will only be allowed to repack;
	///ResidueTypes that do not match the original_residue_type cannot be added.
	///Design disabling takes precedence over SOMETHING, ANDREW TELL ME WHAT IT IS
	bool design_disabled_ = false;

	bool sample_proton_chi_ = true;

	bool ex1_ = false;
	bool ex2_ = false;
	bool ex3_ = false;
	bool ex4_ = false;
	bool ex1aro_ = false;
	bool ex2aro_ = false;
	bool ex1aro_exposed_ = false;
	bool ex2aro_exposed_ = false;
	ExtraRotSample ex1_sample_level_ = NO_EXTRA_CHI_SAMPLES;
	ExtraRotSample ex2_sample_level_ = NO_EXTRA_CHI_SAMPLES;
	ExtraRotSample ex3_sample_level_ = NO_EXTRA_CHI_SAMPLES;
	ExtraRotSample ex4_sample_level_ = NO_EXTRA_CHI_SAMPLES;
	ExtraRotSample ex1aro_sample_level_ = NO_EXTRA_CHI_SAMPLES;
	ExtraRotSample ex2aro_sample_level_ = NO_EXTRA_CHI_SAMPLES;
	ExtraRotSample ex1aro_exposed_sample_level_ = NO_EXTRA_CHI_SAMPLES;
	ExtraRotSample ex2aro_exposed_sample_level_ = NO_EXTRA_CHI_SAMPLES;
	ExtraRotSample exdna_sample_level_ = NO_EXTRA_CHI_SAMPLES;
	Size extrachi_cutoff_ = EXTRACHI_CUTOFF_LIMIT;

	bool operate_on_ex1_ = false;
	bool operate_on_ex2_ = false;
	bool operate_on_ex3_ = false;
	bool operate_on_ex4_ = false;

	rotamer_set::RotamerOperations rotamer_operations_;
	rotamer_set::RotSetOperationList rotsetops_;

	std::vector<std::string> mode_tokens_;

	rna::RNA_ResidueLevelTaskOP rna_task_ = nullptr;

	/// @brief The PackerPalette, which is responsible for initializing the allowed_residue_types_ vector.
	/// @author Vikram K. Mulligan.
	core::pack::palette::PackerPaletteCOP packer_palette_;

#ifdef    SERIALIZATION
public:
	template< class Archive > void save( Archive & arc ) const;
	template< class Archive > void load( Archive & arc );
#endif // SERIALIZATION

};

} //namespace task
} //namespace pack
} //namespace core

#ifdef    SERIALIZATION
CEREAL_FORCE_DYNAMIC_INIT( core_pack_task_ResidueLevelTask_ )
#endif // SERIALIZATION


#endif
