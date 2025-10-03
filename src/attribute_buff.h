/**************************************************************************/
/*  attribute_buff.h                                                      */
/**************************************************************************/
/*                         This file is part of:                          */
/*                        Godot Gameplay Systems                          */
/*              https://github.com/OctoD/godot-gameplay-systems           */
/**************************************************************************/
/* Read the license file in this repo.						              */
/**************************************************************************/

#ifndef GODOT_GAMEPLAY_ATTRIBUTES_ATTRIBUTE_BUFF_H
#define GODOT_GAMEPLAY_ATTRIBUTES_ATTRIBUTE_BUFF_H

#include <godot_cpp/classes/ref_counted.hpp>

using namespace godot;

namespace octod::gameplay::attributes
{
	class AttributeBuffContext;
	class AttributeChangeSet;
	class AttributeContainer;
	class AttributeOperation;
	class RuntimeAttribute;

	class AttributeDiff final : public RefCounted
	{
		GDCLASS(AttributeDiff, RefCounted)

	public:
		String attribute_name;

		double remaining_duration = 0.0;

		bool time_based = false;

		[[nodiscard]] float get_current_buff() const;

		[[nodiscard]] float get_current_value() const;

		[[nodiscard]] bool get_is_forceful() const;

		[[nodiscard]] float get_previous_value() const;

		[[nodiscard]] float get_previous_buff() const;

		void set_current_buff(float p_buff);

		void set_current(float p_current, bool p_forceful_set = false);

		void set_previous(float p_previous);

		void set_previous_buff(float p_previous_buff);

	protected:
		static void _bind_methods();

		float current_buff = 0.0f;

		float current_value = 0.0f;

		bool is_forceful;

		float previous_value = 0.0f;

		float previous_buff = 0.0f;
	};

	class AttributeChangeSetOperation final : public RefCounted
	{
		GDCLASS(AttributeChangeSetOperation, RefCounted)

	public:
		enum AttributeChangeSetOperationTickType : uint8_t
		{
			TICK_MILLISECOND = 0,
			TICK_SECOND = 1,
			TICK_MINUTE = 2,
			TICK_MANUAL = 3,
		};

		struct OperationResult
		{
			bool is_forceful = false;
			float permanent_additive_buff = 0.0;
			float permanent_multiplicative_buff = 1.0f;
			float transient_additive_buff = 0.0;
			float transient_multiplicative_buff = 1.0f;
		};

		[[nodiscard]] bool can_be_processed() const;

		[[nodiscard]] AttributeChangeSet* get_changeset() const;

		[[nodiscard]] float get_duration() const;

		[[nodiscard]] int get_execution_order() const;

		[[nodiscard]] float get_resulting_value() const;

		[[nodiscard]] OperationResult get_resulting_value_struct() const;

		[[nodiscard]] bool is_applied_every_tick() const;

		void reapply_every_tick(bool p_reapplies_every_tick);

		void reset_duration(bool p_resets_duration);

		void set_duration(float p_duration, int p_tick_type = TICK_MILLISECOND);

		void set_execution_order(int p_execution_order = 0);

		void set_remaining_duration(float p_remaining_duration, int p_tick_type = TICK_MILLISECOND);

		void set_transient(bool p_transient);

		void subtract_remaining_duration(float p_subtract_duration);

	protected:
		friend class AttributeChangeSet;
		friend class AttributeOperation;
		friend class AttributeBuffContext;

		static void _bind_methods();

		AttributeChangeSet *change_set = nullptr;

		float duration = 0.0;

		bool executing = false;

		int execution_order = 0;

		int operation_sign = 0;

		float operation_value = 0.0;

		float remaining_duration = 0.0;

		bool reapplies_every_tick = false;

		bool resets_duration = false;

		RuntimeAttribute *runtime_attribute = nullptr;

		AttributeChangeSetOperationTickType tick_type = TICK_MILLISECOND;

		bool transient_operation = false;
	};

	class AttributeChangeSet final : public RefCounted
	{
		GDCLASS(AttributeChangeSet, RefCounted)

	public:
		void clear_persistent_operations();

		void clear_timed_out_operations();

		[[nodiscard]] PackedStringArray get_affected_attributes() const;

		[[nodiscard]] TypedArray<AttributeChangeSetOperation> get_operations() const;

		[[nodiscard]] bool has_operations() const;

		[[nodiscard]] bool is_operating_attribute(const String &p_attribute_name) const;

		[[nodiscard]] Dictionary prepare_diff() const;

		Ref<AttributeChangeSetOperation> operate(const String &p_attribute_name, const AttributeOperation *p_attribute_operation);

		void tick_operations(float p_delta);

	protected:
		friend class AttributeBuffContext;
		friend class AttributeChangeSetOperation;

		static void _bind_methods();

		AttributeBuffContext *attribute_buff_context;

		String change_set_name;

		/// @brief A dictionary where the key is the attribute name and the value is an instance of AttributeChangeSetOperation
		Dictionary operations;
	};

	class AttributeBuffContext final : public RefCounted
	{
		GDCLASS(AttributeBuffContext, RefCounted)

	public:
		void commit(const Ref<AttributeChangeSet> &p_changeset);

		[[nodiscard]] bool has_committed_changesets() const;

		[[nodiscard]] TypedArray<AttributeChangeSet> get_merged_changesets() const;

		[[nodiscard]] TypedArray<AttributeChangeSet> get_merged_changesets_by_name(const String &p_changeset_name) const;

		[[nodiscard]] TypedArray<AttributeChangeSetOperation> get_merged_changeset_operations() const;

		[[nodiscard]] TypedArray<AttributeChangeSetOperation> get_merged_changeset_operations_for_attribute(const String &p_attribute_name) const;

		[[nodiscard]] TypedArray<AttributeChangeSetOperation> get_merged_changeset_operations_for_attribute_with_duration(const String &p_attribute_name) const;

		[[nodiscard]] TypedArray<AttributeChangeSetOperation> get_merged_changeset_operations_with_duration() const;

		/// @brief Returns a Dictionary where the key is the attribute name, and the value is an instance of AttributeDiff
		[[nodiscard]] Dictionary get_diff() const;

		[[nodiscard]] RuntimeAttribute *get_attribute(const String &p_attribute_name) const;

		[[nodiscard]] bool has_attribute(const String &p_attribute_name) const;

		[[nodiscard]] bool has_changeset(const String &p_changeset_name) const;

		/// @brief merges current diffs into the container.
		void merge();

		Ref<AttributeChangeSet> new_changeset(const String &p_changeset_name = "");

		void rollback(const String &p_changeset_name);

		void set_attribute_container(AttributeContainer *p_container);

		void tick_operations(float p_delta, int p_tick_type = AttributeChangeSetOperation::TICK_MILLISECOND);

	protected:
		friend class AttributeChangeSetOperation;
		friend class AttributeChangeSet;

		static void _bind_methods();

		AttributeContainer *attribute_container = nullptr;

		TypedArray<AttributeChangeSet> committed_changesets;

		TypedArray<AttributeChangeSet> merged_changesets;

		TypedArray<AttributeDiff> diffs_to_notify;
	};
} //namespace octod::gameplay::attributes

VARIANT_ENUM_CAST(octod::gameplay::attributes::AttributeChangeSetOperation::AttributeChangeSetOperationTickType)

#endif