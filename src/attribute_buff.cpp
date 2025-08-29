//
// Created by lasagnaking on 8/29/25.
//

#include "attribute_buff.h"
#include "attribute.hpp"
#include "attribute_container.hpp"

using namespace octod::gameplay::attributes;

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

void AttributeChangeSetOperation::set_duration(const float p_duration, int p_unit_of_measure)
{
	duration = p_duration;
	unit_of_measure = static_cast<AttributeChangeSetOperationUnitOfMeasure>(p_unit_of_measure);
}

void AttributeChangeSetOperation::_bind_methods()
{
	/// binds methods to godot
	ClassDB::bind_method(D_METHOD("get_resulting_value"), &AttributeChangeSetOperation::get_resulting_value);
	ClassDB::bind_method(D_METHOD("reapply_every_tick", "reapplies_every_tick"), &AttributeChangeSetOperation::reapply_every_tick);
	ClassDB::bind_method(D_METHOD("reset_duration", "resets_duration"), &AttributeChangeSetOperation::reset_duration);
	ClassDB::bind_method(D_METHOD("set_duration", "duration", "unit_of_measure"), &AttributeChangeSetOperation::set_duration, DEFVAL(UOM_MILLISECOND));
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

Ref<AttributeChangeSetOperation> AttributeChangeSet::operate(const String &p_attribute_name, AttributeOperation *p_attribute_operation)
{
	Ref<AttributeChangeSetOperation> attribute_change_set_operation;

	ERR_FAIL_NULL_V_MSG(attribute_buff_context, attribute_change_set_operation, "This AttributeChangeSet attribute_buff_context is null, this is probably due to a manual instantiation");
	ERR_FAIL_NULL_V_MSG(attribute_buff_context->attribute_container, attribute_change_set_operation, "This AttributeChangeSet attribute_buff_context has a null pointer to attribute_container, this is probably due to a manual instantiation");

	attribute_change_set_operation.instantiate();

	attribute_change_set_operation->attribute_operation = p_attribute_operation;
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
	committed_changesets.append(p_changeset);
}

void AttributeBuffContext::commit_transient(const Ref<AttributeChangeSet> &p_changeset)
{
	ERR_FAIL_COND_MSG(p_changeset.is_null(), "This AttributeChangeSet is null");
	p_changeset->transient_change_set = true;
	committed_changesets.append(p_changeset);
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

Ref<AttributeChangeSet> AttributeBuffContext::new_changeset(const String &p_changeset_name)
{
	Ref<AttributeChangeSet> retval;
	retval.instantiate();

	retval->attribute_buff_context = this;
	retval->change_set_name = p_changeset_name;

	return retval;
}

void AttributeBuffContext::rollback(String p_changeset_name)
{
	/// todo: we need to store changesets on the container first before rolling them back
}

void AttributeBuffContext::set_attribute_container(AttributeContainer *p_container)
{
	attribute_container = p_container;
}

void AttributeBuffContext::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("commit", "changeset"), &AttributeBuffContext::commit);
	ClassDB::bind_method(D_METHOD("commit_transient", "changeset"), &AttributeBuffContext::commit_transient);
	ClassDB::bind_method(D_METHOD("get_attribute", "attribute_name"), &AttributeBuffContext::get_attribute);
	ClassDB::bind_method(D_METHOD("has_attribute", "attribute_name"), &AttributeBuffContext::has_attribute);
	ClassDB::bind_method(D_METHOD("has_changeset", "changeset_name"), &AttributeBuffContext::has_changeset);
	ClassDB::bind_method(D_METHOD("new_changeset", "changeset_name"), &AttributeBuffContext::new_changeset, DEFVAL(""));
	ClassDB::bind_method(D_METHOD("rollback", "changeset_name"), &AttributeBuffContext::rollback);
}