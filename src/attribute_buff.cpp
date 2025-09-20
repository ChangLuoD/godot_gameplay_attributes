/**************************************************************************/
/*  attribute_buff.cpp                                                    */
/**************************************************************************/
/*                         This file is part of:                          */
/*                        Godot Gameplay Systems                          */
/*              https://github.com/OctoD/godot-gameplay-systems           */
/**************************************************************************/
/* Read the license file in this repo.						              */
/**************************************************************************/

// ReSharper disable CppUseStructuredBinding
#include "attribute_buff.h"
#include "attribute.hpp"
#include "attribute_container.hpp"

#ifndef MINUTE
#define MINUTE 60000.00f
#endif

#ifndef SECOND
#define SECOND 1000.00f
#endif

using namespace octod::gameplay::attributes;

bool AttributeDiff::did_change() const
{
	if (is_forceful) {
		return true;
	}

	return !Math::is_equal_approx(current_value + current_buff, previous_value + previous_buff);
}

float AttributeDiff::get_current_buff() const
{
	return current_buff;
}

float AttributeDiff::get_previous_buff() const
{
	return previous_buff;
}

void AttributeDiff::set_current_buff(const float p_buff)
{
	if (is_forceful) {
		return;
	}

	current_buff = p_buff;
}

void AttributeDiff::set_previous_buff(const float p_previous_buff)
{
	if (is_forceful) {
		return;
	}

	previous_buff = p_previous_buff;
}

float AttributeDiff::get_current_value() const
{
	return current_value;
}

bool AttributeDiff::get_is_forceful() const
{
	return is_forceful;
}

float AttributeDiff::get_previous_value() const
{
	return previous_value;
}

void AttributeDiff::set_current(const float p_current, const bool p_forceful_set)
{
	if (is_forceful && !p_forceful_set) {
		return;
	}

	current_value = p_current;
	is_forceful = p_forceful_set;
}

void AttributeDiff::set_previous(const float p_previous)
{
	if (is_forceful) {
		return;
	}

	previous_value = p_previous;
}

void AttributeDiff::_bind_methods()
{
}

bool AttributeChangeSetOperation::can_be_processed() const
{
	if (execution_order == AttributeBuff::QueueExecution::QUEUE_EXECUTION_WATERFALL) {
		TypedArray<AttributeChangeSetOperation> similar_operations = change_set->attribute_buff_context->get_merged_changeset_operations_for_attribute(runtime_attribute->get_attribute_name());

		if (similar_operations.size() == 1) {
			return true;
		}

		for (int i = 0; i < similar_operations.size(); i++) {
			const Ref<AttributeChangeSetOperation> operation = similar_operations[i];

			if (operation->execution_order == AttributeBuff::QueueExecution::QUEUE_EXECUTION_WATERFALL && operation->executing) {
				return false;
			}
		}
	}

	return true;
}

float AttributeChangeSetOperation::get_duration() const
{
	return duration;
}

int AttributeChangeSetOperation::get_execution_order() const
{
	return execution_order;
}

float AttributeChangeSetOperation::get_resulting_value() const
{
	if (runtime_attribute == nullptr) {
		ERR_FAIL_COND_V_MSG(true, 0.0f, "runtime attribute is null on this AttributeChangeSetOperation. This is probably due to a manual instantiation.");
	}

	const OperationResult result = get_resulting_value_struct();

	if (result.is_forceful) {
		return result.permanent_additive_buff;
	}

	return (runtime_attribute->get_value() + result.transient_additive_buff + result.permanent_additive_buff) * (result.transient_multiplicative_buff + result.permanent_multiplicative_buff);
}

AttributeChangeSetOperation::OperationResult AttributeChangeSetOperation::get_resulting_value_struct() const
{
	OperationResult result;

	switch (operation_sign) {
		case OP_ADD:
			if (transient_operation) {
				result.transient_additive_buff += operation_value;
			} else {
				result.permanent_additive_buff += operation_value;
			}
			break;
		case OP_SUBTRACT:
			if (transient_operation) {
				result.transient_additive_buff -= operation_value;
			} else {
				result.permanent_additive_buff -= operation_value;
			}
			break;
		case OP_MULTIPLY:
			if (transient_operation) {
				result.transient_multiplicative_buff *= operation_value;
			} else {
				result.permanent_multiplicative_buff *= operation_value;
			}
			break;
		case OP_DIVIDE:
			{
				if (Math::is_zero_approx(operation_value)) {
					break;
				}

				if (transient_operation) {
					result.transient_multiplicative_buff /= operation_value;
				} else {
					result.permanent_multiplicative_buff /= operation_value;
				}
				break;
			}
		case OP_PERCENTAGE:
			if (transient_operation) {
				result.transient_multiplicative_buff *= operation_value * 100.0f;
			} else {
				result.permanent_multiplicative_buff *= operation_value * 100.0f;
			}
			break;
		case OP_SET:
			result.is_forceful = true;
			result.permanent_additive_buff = operation_value;
			break;
		default:
			break;
	}

	return result;
}

bool AttributeChangeSetOperation::is_applied_every_tick() const
{
	return reapplies_every_tick;
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

	set_remaining_duration(p_duration, p_tick_type);
}

void AttributeChangeSetOperation::set_execution_order(const int p_execution_order)
{
	execution_order = p_execution_order;
}

void AttributeChangeSetOperation::set_remaining_duration(const float p_remaining_duration, const int p_tick_type)
{
	switch (p_tick_type) {
		case TICK_MANUAL:
		case TICK_MILLISECOND:
			remaining_duration = p_remaining_duration;
		case TICK_MINUTE:
			remaining_duration = p_remaining_duration * MINUTE;
		case TICK_SECOND:
			remaining_duration = p_remaining_duration * SECOND;
			break;
		default:
			break;
	}
}

void AttributeChangeSetOperation::set_transient(const bool p_transient)
{
	ERR_FAIL_COND_MSG(!Math::is_zero_approx(duration) && !p_transient, "Cannot mark this operation as not transient if its duration is is not 0.0.");
	transient_operation = p_transient;
}

void AttributeChangeSetOperation::subtract_remaining_duration(const float p_subtract_duration)
{
	remaining_duration -= p_subtract_duration;
}

void AttributeChangeSetOperation::_bind_methods()
{
	/// binds methods to godot
	ClassDB::bind_method(D_METHOD("get_duration"), &AttributeChangeSetOperation::get_duration);
	ClassDB::bind_method(D_METHOD("get_resulting_value"), &AttributeChangeSetOperation::get_resulting_value);
	ClassDB::bind_method(D_METHOD("reapply_every_tick", "reapplies_every_tick"), &AttributeChangeSetOperation::reapply_every_tick);
	ClassDB::bind_method(D_METHOD("reset_duration", "resets_duration"), &AttributeChangeSetOperation::reset_duration);
	ClassDB::bind_method(D_METHOD("set_duration", "duration", "unit_of_measure"), &AttributeChangeSetOperation::set_duration, DEFVAL(TICK_MILLISECOND));
	ClassDB::bind_method(D_METHOD("set_execution_order", "execution_order"), &AttributeChangeSetOperation::set_execution_order, DEFVAL(AttributeBuff::QueueExecution::QUEUE_EXECUTION_PARALLEL));
	ClassDB::bind_method(D_METHOD("set_remaining_duration", "remaining_duration", "unit_of_measure"), &AttributeChangeSetOperation::set_remaining_duration, DEFVAL(TICK_MILLISECOND));
	ClassDB::bind_method(D_METHOD("set_transient", "transient"), &AttributeChangeSetOperation::set_transient);

	/// bind enum constants
	BIND_ENUM_CONSTANT(AttributeChangeSetOperationTickType::TICK_MILLISECOND);
	BIND_ENUM_CONSTANT(AttributeChangeSetOperationTickType::TICK_SECOND);
	BIND_ENUM_CONSTANT(AttributeChangeSetOperationTickType::TICK_MINUTE);
	BIND_ENUM_CONSTANT(AttributeChangeSetOperationTickType::TICK_MANUAL);
}

void AttributeChangeSet::clear_persistent_operations()
{
	TypedArray<AttributeChangeSetOperation> operation_values = get_operations();

	for (int64_t i = operation_values.size() - 1; i >= 0; i--) {
		if (const Ref<AttributeChangeSetOperation> operation = operation_values[i]; operation.is_valid() && !operation->transient_operation) {
			operations.erase(operation);
		}
	}
}

void AttributeChangeSet::clear_timed_out_operations()
{
	TypedArray<AttributeChangeSetOperation> operation_values = get_operations();

	for (int64_t i = operation_values.size() - 1; i >= 0; i--) {
		if (const Ref<AttributeChangeSetOperation> operation = operation_values[i]; operation.is_valid() && !Math::is_zero_approx(operation->duration) && Math::is_zero_approx(operation->remaining_duration)) {
			operations.erase(operation);
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
	PackedStringArray affected_attributes = operations.keys();

	for (const Ref<AttributeChangeSetOperation> operation : operations.values()) {
		const String &attribute_name = operation->runtime_attribute->get_attribute_name();
		Ref<AttributeDiff> attribute_diff;
		attribute_diff.instantiate();

		ERR_FAIL_NULL_V_MSG(operation, diff, "Operation on " + attribute_name + " does not exist.");

		attribute_diff->attribute_name = attribute_name;

		if (!Math::is_zero_approx(operation->duration)) {
			attribute_diff->time_based = true;

			switch (operation->tick_type) {
				case AttributeChangeSetOperation::TICK_MANUAL:
				case AttributeChangeSetOperation::TICK_MILLISECOND:
					attribute_diff->remaining_duration = operation->duration;
					break;
				case AttributeChangeSetOperation::TICK_SECOND:
					attribute_diff->remaining_duration = operation->duration * SECOND;
					break;
				case AttributeChangeSetOperation::TICK_MINUTE:
					attribute_diff->remaining_duration = operation->duration * MINUTE;
					break;
			}
		}

		const AttributeChangeSetOperation::OperationResult result = operation->get_resulting_value_struct();

		attribute_diff->set_current_buff(result.transient_additive_buff * result.transient_multiplicative_buff);
		attribute_diff->set_current(result.permanent_additive_buff * result.permanent_multiplicative_buff, result.is_forceful);
		attribute_diff->set_previous(attribute_buff_context->get_attribute(attribute_name)->get_value());
		attribute_diff->set_previous_buff(attribute_buff_context->get_attribute(attribute_name)->get_buff());

		if (attribute_diff->did_change()) {
			diff.set(attribute_name, attribute_diff);
		}
	}

	return diff;
}

Ref<AttributeChangeSetOperation> AttributeChangeSet::operate(const String &p_attribute_name, const AttributeOperation *p_attribute_operation)
{
	Ref<AttributeChangeSetOperation> attribute_change_set_operation;

	ERR_FAIL_NULL_V_MSG(attribute_buff_context, attribute_change_set_operation, "This AttributeChangeSet attribute_buff_context is null, this is probably due to a manual instantiation");
	ERR_FAIL_NULL_V_MSG(attribute_buff_context->attribute_container, attribute_change_set_operation, "This AttributeChangeSet attribute_buff_context has a null pointer to attribute_container, this is probably due to a manual instantiation");
	ERR_FAIL_NULL_V_MSG(p_attribute_operation, attribute_change_set_operation, "AttributeOperation is null");

	attribute_change_set_operation.instantiate();

	attribute_change_set_operation->change_set = this;
	attribute_change_set_operation->operation_sign = p_attribute_operation->get_operand();
	attribute_change_set_operation->operation_value = p_attribute_operation->get_value();
	attribute_change_set_operation->runtime_attribute = attribute_buff_context->attribute_container->get_runtime_attribute_by_name(p_attribute_name).ptr();

	operations.set(p_attribute_name, attribute_change_set_operation);

	return attribute_change_set_operation;
}

void AttributeChangeSet::tick_operations(const float p_delta)
{
	TypedArray<AttributeChangeSetOperation> operation_values = get_operations();

	for (int64_t i = operation_values.size() - 1; i >= 0; i--) {
		if (const Ref<AttributeChangeSetOperation> operation = operation_values[i]; !Math::is_zero_approx(operation->duration)) {
			operation->subtract_remaining_duration(p_delta);

			if (Math::is_zero_approx(operation->remaining_duration) || Math::is_zero_approx(operation->remaining_duration)) {
				operations.erase(operation);
			}
		}
	}
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
	ERR_FAIL_COND_MSG(!p_changeset->has_operations(), "This AttributeChangeSet was discarded since it has no operations to commit.");

	committed_changesets.append(p_changeset);
}

bool AttributeBuffContext::has_committed_changesets() const
{
	return committed_changesets.size() > 0;
}

TypedArray<AttributeChangeSet> AttributeBuffContext::get_merged_changesets() const
{
	return merged_changesets;
}

TypedArray<AttributeChangeSet> AttributeBuffContext::get_merged_changesets_by_name(const String &p_changeset_name) const
{
	TypedArray<AttributeChangeSet> subset;

	for (int64_t i = 0; i < merged_changesets.size(); i++) {
		if (Ref<AttributeChangeSet> changeset = merged_changesets[i]; changeset->change_set_name == p_changeset_name) {
			subset.append(changeset);
		}
	}

	return subset;
}

TypedArray<AttributeChangeSetOperation> AttributeBuffContext::get_merged_changeset_operations() const
{
	TypedArray<AttributeChangeSetOperation> operations;

	for (int i = 0; i < merged_changesets.size(); i++) {
		const Ref<AttributeChangeSet> changeset = merged_changesets[i];
		const TypedArray<AttributeChangeSetOperation> changeset_operations = changeset->get_operations();
		operations.append_array(changeset_operations);
	}

	return operations;
}

TypedArray<AttributeChangeSetOperation> AttributeBuffContext::get_merged_changeset_operations_for_attribute(const String &p_attribute_name) const
{
	TypedArray<AttributeChangeSetOperation> operations;

	for (int i = 0; i < merged_changesets.size(); i++) {
		const Ref<AttributeChangeSet> changeset = merged_changesets[i];
		const TypedArray<AttributeChangeSetOperation> changeset_operations = changeset->get_operations();

		for (int j = 0; j < changeset_operations.size(); j++) {
			if (const Ref<AttributeChangeSetOperation> operation = changeset_operations[j]; operation->runtime_attribute->get_attribute_name() == p_attribute_name) {
				operations.append(operation);
			}
		}
	}

	return operations;
}

TypedArray<AttributeChangeSetOperation> AttributeBuffContext::get_merged_changeset_operations_for_attribute_with_duration(const String &p_attribute_name) const
{
	TypedArray<AttributeChangeSetOperation> operations;

	for (int i = 0; i < merged_changesets.size(); i++) {
		const Ref<AttributeChangeSet> changeset = merged_changesets[i];
		const TypedArray<AttributeChangeSetOperation> changeset_operations = changeset->get_operations();

		for (int j = 0; j < changeset_operations.size(); j++) {
			if (const Ref<AttributeChangeSetOperation> operation = changeset_operations[j]; operation->runtime_attribute->get_attribute_name() == p_attribute_name && !Math::is_zero_approx(operation->duration)) {
				operations.append(operation);
			}
		}
	}

	return operations;
}

TypedArray<AttributeChangeSetOperation> AttributeBuffContext::get_merged_changeset_operations_with_duration() const
{
	TypedArray<AttributeChangeSetOperation> operations;

	for (int i = 0; i < merged_changesets.size(); i++) {
		const Ref<AttributeChangeSet> changeset = merged_changesets[i];
		const TypedArray<AttributeChangeSetOperation> changeset_operations = changeset->get_operations();

		for (int j = 0; j < changeset_operations.size(); j++) {
			if (const Ref<AttributeChangeSetOperation> operation = changeset_operations[j]; !Math::is_zero_approx(operation->duration)) {
				operations.append(operation);
			}
		}
	}

	return operations;
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
				if (attribute_diff->get_is_forceful()) {
					stored_attribute_diff->set_current(attribute_diff->get_current_value(), true);
				} else {
					stored_attribute_diff->set_current_buff(stored_attribute_diff->get_current_buff() + attribute_diff->get_current_buff());
					stored_attribute_diff->set_current(stored_attribute_diff->get_current_value() + attribute_diff->get_current_value());
				}
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

	for (int i = 0; i < merged_changesets.size(); i++) {
		if (const Ref<AttributeChangeSet> p_changeset = merged_changesets[i]; p_changeset->change_set_name == p_changeset_name) {
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

	for (int64_t i = 0; i < affected_attributes.size(); i++) {
		const String &attribute_name = affected_attributes[i];
		const Ref<AttributeDiff> attribute_diff = diff[attribute_name];
		const Ref<RuntimeAttribute> runtime_attribute = attribute_container->get_runtime_attribute_by_name(attribute_name);

		if (attribute_diff->did_change()) {
			attribute_diff->attribute_name = attribute_name;
			diffs_to_notify.push_back(attribute_diff);
		}
	}

	for (int64_t i = committed_changesets.size() - 1; i >= 0; i--) {
		const Ref<AttributeChangeSet> changeset = committed_changesets[i];

		changeset->clear_persistent_operations();

		if (changeset->has_operations()) {
			merged_changesets.append(changeset);
		} else {
			committed_changesets.remove_at(i);
		}
	}

	for (int i = 0; i < diffs_to_notify.size(); i++) {
		if (const Ref<AttributeDiff> attribute_diff = diffs_to_notify[i]; attribute_diff->did_change()) {
			attribute_container->alter_attribute(
					attribute_diff->attribute_name,
					attribute_diff->get_current_buff(),
					attribute_diff->get_current_value(),
					attribute_diff->get_is_forceful());
		}
	}

	committed_changesets.clear();
	diffs_to_notify.clear();
}

Ref<AttributeChangeSet> AttributeBuffContext::new_changeset(const String &p_changeset_name)
{
	Ref<AttributeChangeSet> retval;
	retval.instantiate();

	retval->attribute_buff_context = this;
	retval->change_set_name = p_changeset_name;

	return retval;
}

void AttributeBuffContext::rollback(const String &p_changeset_name)
{
	ERR_FAIL_NULL_MSG(attribute_container, "AttributeContainer is null, cannot rollback. This is probably due to a manual instantiation of AttributeBuffContext.");

	for (int64_t i = merged_changesets.size() - 1; i >= 0; i--) {
		if (const Ref<AttributeChangeSet> changeset = merged_changesets[i]; changeset->change_set_name == p_changeset_name) {
			const Dictionary diff = changeset->prepare_diff();
			const PackedStringArray affected_attributes = diff.keys();

			for (int j = 0; j < affected_attributes.size(); j++) {
				const String &attribute_name = affected_attributes[j];
				const Ref<AttributeDiff> attribute_diff = diff[attribute_name];

				attribute_container->alter_attribute(attribute_name, -attribute_diff->get_current_buff(), 0.0f, false);
			}

			merged_changesets.erase(i);
			break;
		}
	}

	merge();
}

void AttributeBuffContext::set_attribute_container(AttributeContainer *p_container)
{
	attribute_container = p_container;
}

void AttributeBuffContext::tick_operations(const float p_delta, const int p_tick_type)
{
	for (int64_t i = merged_changesets.size() - 1; i >= 0; i--) {
		if (const Ref<AttributeChangeSet> changeset = merged_changesets[i]; changeset->has_operations()) {
			changeset->tick_operations(p_delta);
		}
	}
}

void AttributeBuffContext::_bind_methods()
{
	/// binds methods to godot
	ClassDB::bind_method(D_METHOD("commit", "changeset"), &AttributeBuffContext::commit);
	ClassDB::bind_method(D_METHOD("get_attribute", "attribute_name"), &AttributeBuffContext::get_attribute);
	ClassDB::bind_method(D_METHOD("has_attribute", "attribute_name"), &AttributeBuffContext::has_attribute);
	ClassDB::bind_method(D_METHOD("has_changeset", "changeset_name"), &AttributeBuffContext::has_changeset);
	ClassDB::bind_method(D_METHOD("new_changeset", "changeset_name"), &AttributeBuffContext::new_changeset, DEFVAL(""));
	ClassDB::bind_method(D_METHOD("rollback", "changeset_name"), &AttributeBuffContext::rollback);

	/// adds signals (used by the debugger tools mostly)
}