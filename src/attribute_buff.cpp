/**************************************************************************/
/*  attribute_buff.cpp                                                    */
/**************************************************************************/
/*                         This file is part of:                          */
/*                        Godot Gameplay Systems                          */
/*              https://github.com/OctoD/godot-gameplay-systems           */
/**************************************************************************/
/* Read the license file in this repo.						              */
/**************************************************************************/

#include "attribute_buff.h"
#include "attribute.hpp"
#include "attribute_container.hpp"

using namespace octod::gameplay::attributes;

bool AttributeDiff::did_change() const
{
	return Math::is_equal_approx(current, previous);
}

float AttributeDiff::get_buff() const
{
	return buff;
}

float AttributeDiff::get_previous_buff() const
{
	return previous_buff;
}

void AttributeDiff::set_buff(const float p_buff)
{
	buff = p_buff;
}

void AttributeDiff::set_previous_buff(const float p_previous_buff)
{
	previous_buff = p_previous_buff;
}

float AttributeDiff::get_current() const
{
	return current;
}

float AttributeDiff::get_previous() const
{
	return previous;
}

void AttributeDiff::set_current(const float p_current)
{
	current = p_current;
}

void AttributeDiff::set_previous(const float p_previous)
{
	previous = p_previous;
}

void AttributeDiff::_bind_methods()
{
	/// binds methods to godot
	ClassDB::bind_method(D_METHOD("did_change"), &AttributeDiff::did_change);
	ClassDB::bind_method(D_METHOD("get_buff"), &AttributeDiff::get_buff);
	ClassDB::bind_method(D_METHOD("get_current"), &AttributeDiff::get_current);
	ClassDB::bind_method(D_METHOD("get_previous"), &AttributeDiff::get_previous);
	ClassDB::bind_method(D_METHOD("get_previous_buff"), &AttributeDiff::get_previous_buff);
	ClassDB::bind_method(D_METHOD("set_buff", "buff"), &AttributeDiff::set_buff);
	ClassDB::bind_method(D_METHOD("set_current", "current"), &AttributeDiff::set_current);
	ClassDB::bind_method(D_METHOD("set_previous", "previous"), &AttributeDiff::set_previous);
	ClassDB::bind_method(D_METHOD("set_previous_buff", "previous_buff"), &AttributeDiff::set_previous_buff);

	/// binds properties to godot
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "buff"), "set_buff", "get_buff");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "current"), "set_current", "get_current");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "previous"), "set_previous", "get_previous");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "previous_buff"), "set_previous_buff", "get_previous_buff");
}

float AttributeChangeSetOperation::get_resulting_value() const
{
	if (runtime_attribute && attribute_operation) {
		return attribute_operation->operate(runtime_attribute->get_buffed_value());
	}

	return 0.0;
}

void AttributeChangeSetOperation::reapply_every_tick(const bool p_reapplies_every_tick)
{
	reapplies_every_tick = p_reapplies_every_tick;
}

void AttributeChangeSetOperation::reset_duration(const bool p_resets_duration)
{
	resets_duration = p_resets_duration;
}

void AttributeChangeSetOperation::set_duration(const float p_duration, int p_tick_type)
{
	duration = p_duration;
	tick_type = static_cast<AttributeChangeSetOperationTickType>(p_tick_type);
	transient_operation = !Math::is_zero_approx(p_duration);
}

void AttributeChangeSetOperation::set_transient(const bool p_transient)
{
	ERR_FAIL_COND_MSG(!Math::is_zero_approx(duration) && !p_transient, "Cannot mark this operation as not transient if its duration is is not 0.0.");
	transient_operation = p_transient;
}

void AttributeChangeSetOperation::_bind_methods()
{
	/// binds methods to godot
	ClassDB::bind_method(D_METHOD("get_resulting_value"), &AttributeChangeSetOperation::get_resulting_value);
	ClassDB::bind_method(D_METHOD("reapply_every_tick", "reapplies_every_tick"), &AttributeChangeSetOperation::reapply_every_tick);
	ClassDB::bind_method(D_METHOD("reset_duration", "resets_duration"), &AttributeChangeSetOperation::reset_duration);
	ClassDB::bind_method(D_METHOD("set_duration", "duration", "unit_of_measure"), &AttributeChangeSetOperation::set_duration, DEFVAL(TICK_MILLISECOND));
	ClassDB::bind_method(D_METHOD("set_transient", "transient"), &AttributeChangeSetOperation::set_transient);
}

void AttributeChangeSet::clear_persistent_operations()
{
	for (int64_t i = operations.size() - 1; i >= 0; i--) {
		if (const Ref<AttributeChangeSetOperation> operation = operations[i]; !operation->transient_operation) {
			operations.erase(i);
		}
	}
}

PackedStringArray AttributeChangeSet::get_affected_attributes() const
{
	return operations.keys();
}

TypedArray<AttributeChangeSetOperation> AttributeChangeSet::get_operations() const
{
	return operations.values();
}

bool AttributeChangeSet::has_operations() const
{
	return operations.size() > 0;
}

bool AttributeChangeSet::is_operating_attribute(const String &p_attribute_name) const
{
	return operations.has(p_attribute_name);
}

Dictionary AttributeChangeSet::prepare_diff() const
{
	Dictionary diff;
	PackedStringArray affected_attributes = get_affected_attributes();

	for (int i = 0; i < affected_attributes.size(); i++) {
		const String &attribute_name = affected_attributes[i];
		const Ref<AttributeChangeSetOperation> operation = operations[attribute_name];
		Ref<AttributeDiff> attribute_diff;
		attribute_diff.instantiate();
		diff.set(attribute_name, attribute_diff);

		if (!Math::is_zero_approx(operation->duration)) {
			attribute_diff->time_based = true;

			switch (operation->tick_type) {
				case AttributeChangeSetOperation::TICK_MILLISECOND:
					attribute_diff->remaining_duration = operation->duration;
					break;
				case AttributeChangeSetOperation::TICK_SECOND:
					attribute_diff->remaining_duration = operation->duration * 1000.0;
					break;
				case AttributeChangeSetOperation::TICK_MINUTE:
					attribute_diff->remaining_duration = operation->duration * 60000.0;
					break;
			}
		}

		float permanent_additive_buff = 0.0;
		float permanent_multiplicative_buff = 1.0;
		float transient_additive_buff = 0.0;
		float transient_multiplicative_buff = 1.0;

		switch (operation->attribute_operation->get_operand()) {
			case OP_ADD:
			case OP_SUBTRACT:
				if (operation->transient_operation) {
					transient_additive_buff = operation->attribute_operation->operate(transient_additive_buff);
				} else {
					permanent_additive_buff = operation->attribute_operation->operate(permanent_additive_buff);
				}
				break;
			case OP_MULTIPLY:
			case OP_DIVIDE:
			case OP_PERCENTAGE:
				if (operation->transient_operation) {
					transient_multiplicative_buff = operation->attribute_operation->operate(transient_multiplicative_buff);
				} else {
					permanent_multiplicative_buff = operation->attribute_operation->operate(permanent_multiplicative_buff);
				}
				break;
			default:
				break;
		}

		attribute_diff->set_buff(transient_additive_buff + (operation->runtime_attribute->get_value() * transient_multiplicative_buff));
		attribute_diff->set_current((permanent_additive_buff * permanent_multiplicative_buff));
		attribute_diff->set_previous(attribute_buff_context->get_attribute(attribute_name)->get_value());
		attribute_diff->set_previous_buff(attribute_buff_context->get_attribute(attribute_name)->get_buff());
	}

	return diff;
}

Ref<AttributeChangeSetOperation> AttributeChangeSet::operate(const String &p_attribute_name, AttributeOperation *p_attribute_operation)
{
	Ref<AttributeChangeSetOperation> attribute_change_set_operation;

	ERR_FAIL_NULL_V_MSG(attribute_buff_context, attribute_change_set_operation, "This AttributeChangeSet attribute_buff_context is null, this is probably due to a manual instantiation");
	ERR_FAIL_NULL_V_MSG(attribute_buff_context->attribute_container, attribute_change_set_operation, "This AttributeChangeSet attribute_buff_context has a null pointer to attribute_container, this is probably due to a manual instantiation");

	attribute_change_set_operation.instantiate();

	attribute_change_set_operation->attribute_operation = p_attribute_operation;
	attribute_change_set_operation->change_set = this;
	attribute_change_set_operation->runtime_attribute = attribute_buff_context->attribute_container->get_runtime_attribute_by_name(p_attribute_name).ptr();

	operations.set(p_attribute_name, attribute_change_set_operation);

	return attribute_change_set_operation;
}

void AttributeChangeSet::_bind_methods()
{
	/// binds methods to godot
	ClassDB::bind_method(D_METHOD("get_operations"), &AttributeChangeSet::get_operations);
	ClassDB::bind_method(D_METHOD("has_operations"), &AttributeChangeSet::has_operations);
	ClassDB::bind_method(D_METHOD("is_operating_attribute"), &AttributeChangeSet::is_operating_attribute);
	ClassDB::bind_method(D_METHOD("operate", "attribute_name", "attribute_operation"), &AttributeChangeSet::operate);
}

void AttributeBuffContext::commit(const Ref<AttributeChangeSet> &p_changeset)
{
	ERR_FAIL_COND_MSG(p_changeset.is_null(), "This AttributeChangeSet is null");

	if (!p_changeset->has_operations()) {
		WARN_PRINT("This AttributeChangeSet was discarded since it has no operations to commit.");
		return;
	}

	committed_changesets.append(p_changeset);
}

Dictionary AttributeBuffContext::get_diff() const
{
	Dictionary attributes_diff;

	for (int i = 0; i < committed_changesets.size(); i++) {
		const Ref<AttributeChangeSet> changeset = committed_changesets[i];
		const Dictionary &changeset_diff = changeset->prepare_diff();

		PackedStringArray affected_attributes = changeset_diff.keys();

		for (int j = 0; j < affected_attributes.size(); j++) {
			const String &attribute_name = affected_attributes[j];
			const Ref<AttributeDiff> attribute_diff = changeset_diff[attribute_name];
			const Ref<AttributeDiff> stored_attribute_diff = attributes_diff.get_or_add(attribute_name, attribute_diff);

			if (i > 0) {
				stored_attribute_diff->set_buff(stored_attribute_diff->get_buff() + attribute_diff->get_buff());
				stored_attribute_diff->set_current(stored_attribute_diff->get_current() + attribute_diff->get_current());
			}
		}
	}

	return attributes_diff;
}

RuntimeAttribute *AttributeBuffContext::get_attribute(const String &p_attribute_name) const
{
	ERR_FAIL_NULL_V_MSG(attribute_container, nullptr, "This AttributeChangeSet attribute_container pointer is null, this is probably due to a manual instantiation");

	const Ref<RuntimeAttribute> runtime_attribute = attribute_container->get_runtime_attribute_by_name(p_attribute_name);

	return runtime_attribute.is_valid() ? runtime_attribute.ptr() : nullptr;
}

bool AttributeBuffContext::has_attribute(const String &p_attribute_name) const
{
	ERR_FAIL_NULL_V_MSG(attribute_container, false, "This AttributeChangeSet attribute_container pointer is null, this is probably due to a manual instantiation");
	const Ref<RuntimeAttribute> runtime_attribute = attribute_container->get_runtime_attribute_by_name(p_attribute_name);
	return runtime_attribute.is_valid();
}

bool AttributeBuffContext::has_changeset(const String &p_changeset_name) const
{
	if (p_changeset_name.is_empty()) {
		return false;
	}

	for (int i = 0; i < committed_changesets.size(); i++) {
		if (const Ref<AttributeChangeSet> p_changeset = committed_changesets[i]; p_changeset->change_set_name == p_changeset_name) {
			return true;
		}
	}

	return false;
}

void AttributeBuffContext::merge()
{
	ERR_FAIL_NULL_MSG(attribute_container, "AttributeContainer is null, cannot merge commits. This is probably due to a manual instantiation of AttributeBuffContext.");

	const Dictionary &diff = get_diff();
	PackedStringArray affected_attributes = diff.keys();

	for (int i = 0; i < affected_attributes.size(); i++) {
		const String &attribute_name = affected_attributes[i];
		const Ref<AttributeDiff> attribute_diff = diff[attribute_name];
		const Ref<RuntimeAttribute> runtime_attribute = attribute_container->get_runtime_attribute_by_name(attribute_name);

		runtime_attribute->set_buff(attribute_diff->get_buff());
		runtime_attribute->set_value(attribute_diff->get_current());

		attribute_diff->attribute_name = attribute_name;

		diffs_to_notify.push_back(attribute_diff);
	}

	for (int64_t i = committed_changesets.size() - 1; i >= 0; i--) {
		const Ref<AttributeChangeSet> changeset = committed_changesets[i];
		changeset->clear_persistent_operations();

		if (!changeset->has_operations()) {
			committed_changesets.erase(i);
		}
	}

	notify_attributes_container();
}

Ref<AttributeChangeSet> AttributeBuffContext::new_changeset(const String &p_changeset_name)
{
	Ref<AttributeChangeSet> retval;
	retval.instantiate();

	retval->attribute_buff_context = this;
	retval->change_set_name = p_changeset_name;

	return retval;
}

void AttributeBuffContext::notify_attributes_container()
{
	for (int i = 0; i < diffs_to_notify.size(); i++) {
		if (const Ref<AttributeDiff> attribute_diff = diffs_to_notify[i]; attribute_diff->did_change()) {
			Ref<RuntimeAttribute> runtime_attribute = attribute_container->get_runtime_attribute_by_name(attribute_diff->attribute_name);

			attribute_container->emit_signal(
					"attribute_changed",
					runtime_attribute,
					attribute_diff->get_previous() + attribute_diff->get_previous_buff(),
					attribute_diff->get_current() + attribute_diff->get_buff()
					);
		}
	}

	diffs_to_notify.clear();
}

void AttributeBuffContext::rollback(const String &p_changeset_name)
{
	ERR_FAIL_NULL_MSG(attribute_container, "AttributeContainer is null, cannot rollback. This is probably due to a manual instantiation of AttributeBuffContext.");

	for (int64_t i = committed_changesets.size() - 1; i >= 0; i--) {
		if (const Ref<AttributeChangeSet> changeset = committed_changesets[i]; changeset->change_set_name == p_changeset_name) {
			const Dictionary diff = changeset->prepare_diff();
			const PackedStringArray affected_attributes = diff.keys();

			for (int j = 0; j < affected_attributes.size(); j++) {
				const String &attribute_name = affected_attributes[j];
				const Ref<RuntimeAttribute> runtime_attribute = attribute_container->get_runtime_attribute_by_name(attribute_name);
				const Ref<AttributeDiff> attribute_diff = diff[attribute_name];

				runtime_attribute->set_buff(runtime_attribute->get_buff() - attribute_diff->get_buff());
			}

			committed_changesets.erase(i);
			break;
		}
	}

	merge();
}

void AttributeBuffContext::set_attribute_container(AttributeContainer *p_container)
{
	attribute_container = p_container;
}

void AttributeBuffContext::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("commit", "changeset"), &AttributeBuffContext::commit);
	ClassDB::bind_method(D_METHOD("get_attribute", "attribute_name"), &AttributeBuffContext::get_attribute);
	ClassDB::bind_method(D_METHOD("has_attribute", "attribute_name"), &AttributeBuffContext::has_attribute);
	ClassDB::bind_method(D_METHOD("has_changeset", "changeset_name"), &AttributeBuffContext::has_changeset);
	ClassDB::bind_method(D_METHOD("new_changeset", "changeset_name"), &AttributeBuffContext::new_changeset, DEFVAL(""));
	ClassDB::bind_method(D_METHOD("rollback", "changeset_name"), &AttributeBuffContext::rollback);
}