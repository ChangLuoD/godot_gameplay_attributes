/**************************************************************************/
/*  attribute_buff.hpp                                                    */
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
	class AttributeContainer;
	class AttributeOperation;
	class RuntimeAttribute;

	enum AttributeChangeSetOperationUnitOfMeasure : uint8_t
	{
		UOM_MILLISECOND = 0,
		UOM_SECOND = 1,
		UOM_MINUTE = 2,
	};

	class AttributeChangeSetOperation final : public RefCounted
	{
		GDCLASS(AttributeChangeSetOperation, RefCounted)

	public:
		[[nodiscard]] float get_resulting_value() const;

		void reapply_every_tick(bool p_reapplies_every_tick);

		void reset_duration(bool p_resets_duration);

		void set_duration(float p_duration, int p_unit_of_measure = UOM_MILLISECOND);

	protected:
		friend class AttributeChangeSet;

		static void _bind_methods();

		AttributeOperation *attribute_operation = nullptr;

		bool reapplies_every_tick = false;

		bool resets_duration = false;

		float duration = 0.0;

		RuntimeAttribute *runtime_attribute = nullptr;

		AttributeChangeSetOperationUnitOfMeasure unit_of_measure = UOM_MILLISECOND;
	};

	class AttributeChangeSet final : public RefCounted
	{
		GDCLASS(AttributeChangeSet, RefCounted)

	public:
		[[nodiscard]] TypedArray<AttributeChangeSetOperation> get_operations() const;

		[[nodiscard]] bool has_operations() const;

		[[nodiscard]] bool is_operating_attribute(const String &p_attribute_name) const;

		Ref<AttributeChangeSetOperation> operate(const String &p_attribute_name, AttributeOperation *p_attribute_operation);

	protected:
		friend class AttributeBuffContext;

		static void _bind_methods();

		AttributeBuffContext *attribute_buff_context;

		String change_set_name;

		Dictionary operations;

		bool transient_change_set = false;
	};

	class AttributeBuffContext final : public RefCounted
	{
		GDCLASS(AttributeBuffContext, RefCounted)

	public:
		void commit(const Ref<AttributeChangeSet> &p_changeset);

		void commit_transient(const Ref<AttributeChangeSet> &p_changeset);

		RuntimeAttribute *get_attribute(const String &p_attribute_name) const;

		[[nodiscard]] bool has_attribute(const String &p_attribute_name) const;

		[[nodiscard]] bool has_changeset(const String &p_changeset_name) const;

		Ref<AttributeChangeSet> new_changeset(const String &p_changeset_name = "");

		void rollback(String p_changeset_name);

		void set_attribute_container(AttributeContainer *p_container);

	protected:
		friend class AttributeChangeSet;

		static void _bind_methods();

		AttributeContainer *attribute_container = nullptr;

		TypedArray<AttributeChangeSet> committed_changesets;
	};
} //namespace octod::gameplay::attributes

VARIANT_ENUM_CAST(octod::gameplay::attributes::AttributeChangeSetOperationUnitOfMeasure)

#endif