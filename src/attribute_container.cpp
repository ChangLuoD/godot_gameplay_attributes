/**************************************************************************/
/*  attribute_container.cpp                                               */
/**************************************************************************/
/*                         This file is part of:                          */
/*                        Godot Gameplay Systems                          */
/*              https://github.com/OctoD/godot-gameplay-systems           */
/**************************************************************************/
/* Read the license file in this repo.						              */
/**************************************************************************/

#include "attribute_container.hpp"

#include "attribute.hpp"
#include "godot_cpp/classes/wrapped.hpp"

using namespace octod::gameplay::attributes;

void AttributeContainer::_notification(const int p_what)
{
	if (p_what == NOTIFICATION_ENTER_TREE) {
		setup();
		set_physics_process(true);
	} else if (p_what == NOTIFICATION_PHYSICS_PROCESS && !manual_ticking) {
		buff_context->tick_operations(static_cast<float>(get_physics_process_delta_time()));
	}
}

void AttributeContainer::_on_attribute_changed(const Ref<RuntimeAttribute> &p_attribute, const float p_previous_value, const float p_new_value)
{
	emit_signal("attribute_changed", p_attribute, p_previous_value, p_new_value);
	notify_derived_attributes(p_attribute);
}

void AttributeContainer::_on_buff_applied(RuntimeBuff *p_buff)
{
	emit_signal("buff_applied", Ref(p_buff));

	if (const auto attribute = get_runtime_attribute_by_name(p_buff->get_attribute_name()); attribute.is_valid()) {
		notify_derived_attributes(attribute);
	}
}

void AttributeContainer::_on_buff_removed(const Ref<RuntimeBuff> &p_buff)
{
	emit_signal("buff_removed", p_buff);

	if (const auto attribute = get_runtime_attribute_by_name(p_buff->get_attribute_name()); attribute.is_valid() && !attribute.is_null()) {
		notify_derived_attributes(attribute);
	}
}

void AttributeContainer::_on_buff_time_updated(const Ref<RuntimeBuff> &p_buff)
{
	emit_signal("buff_time_updated", p_buff);
}

bool AttributeContainer::has_attribute(const Ref<AttributeBase> &p_attribute) const
{
	return attributes.has(p_attribute->get_attribute_name());
}

void AttributeContainer::notify_derived_attributes(const Ref<RuntimeAttribute> &p_base_runtime_attribute)
{
	if (derived_attributes.has(p_base_runtime_attribute->get_attribute()->get_attribute_name())) {
		TypedArray<RuntimeAttribute> derived = derived_attributes[p_base_runtime_attribute->get_attribute()->get_attribute_name()];

		for (int i = 0; i < derived.size(); i++) {
			const Ref<RuntimeAttribute> derived_attribute = derived[i];
			derived_attribute->compute_value();
		}
	}
}

void AttributeContainer::add_attribute(const Ref<AttributeBase> &p_attribute)
{
	ERR_FAIL_NULL_MSG(p_attribute, "Attribute cannot be null, it must be an instance of a class inheriting from AttributeBase abstract class.");
	ERR_FAIL_COND_MSG(has_attribute(p_attribute), "Attribute already exists in the container.");

	RuntimeAttribute *runtime_attribute = memnew(RuntimeAttribute);

	if (attribute_set.is_null()) {
		attribute_set.instantiate();
	}

	if (!attribute_set->has_attribute(p_attribute)) {
		attribute_set->add_attribute(p_attribute);
	}

	runtime_attribute->attribute_container = this;
	runtime_attribute->set_attribute(p_attribute);
	runtime_attribute->set_attribute_set(attribute_set);

	if (TypedArray<AttributeBase> base_attributes = runtime_attribute->get_derived_from(); base_attributes.size() > 0) {
		for (int i = 0; i < base_attributes.size(); i++) {
			const Ref<AttributeBase> base_attribute = base_attributes[i];

			ERR_FAIL_COND_MSG(base_attribute.is_null(), "Required base attribute " + p_attribute->get_attribute_name() + " does not exist into the AttributeSet.");

			if (derived_attributes.has(base_attribute->get_attribute_name())) {
				Array _derived = derived_attributes[base_attribute->get_attribute_name()];
				_derived.push_back(runtime_attribute);
			} else {
				Array _derived;
				derived_attributes[base_attribute->get_attribute_name()] = _derived;
				_derived.push_back(runtime_attribute);
			}
		}
	}

	const Callable attribute_changed_callable = Callable::create(this, "_on_attribute_changed");
	const Callable buff_applied_callable = Callable::create(this, "_on_buff_applied");
	const Callable buff_removed_callable = Callable::create(this, "_on_buff_removed");
	const Callable buff_time_updated_callable = Callable::create(this, "_on_buff_time_updated");

	runtime_attribute->connect("attribute_changed", attribute_changed_callable);
	runtime_attribute->connect("buff_added", buff_applied_callable);
	runtime_attribute->connect("buff_removed", buff_removed_callable);
	runtime_attribute->connect("buff_time_updated", buff_time_updated_callable);

	attributes[p_attribute->get_attribute_name()] = runtime_attribute;
}

void AttributeContainer::apply_buff(const Ref<AttributeBuff> &p_buff) const
{
	ERR_FAIL_NULL_MSG(p_buff, "Buff cannot be null, it must be an instance of a class inheriting from AttributeBuff abstract class.");

	if (p_buff->is_operate_overridden()) {
		/// disclaimer: this is going to be removed in the future for the sake of simplicity.
		TypedArray<AttributeBase> _attributes;
		TypedArray<RuntimeAttribute> _affected_runtime_attributes;
		TypedArray<float> buffed_values;

		ERR_FAIL_COND_MSG(!GDVIRTUAL_IS_OVERRIDDEN_PTR(p_buff, _applies_to), "Buff must override the _applies_to method to apply to derived attributes.");
		ERR_FAIL_COND_MSG(!GDVIRTUAL_CALL_PTR(p_buff, _applies_to, attribute_set, _attributes), "An error occurred calling the overridden _applies_to method.");

		for (int i = 0; i < _attributes.size(); i++) {
			const Ref<AttributeBase> attribute_base = _attributes[i];
			Ref<RuntimeAttribute> attribute = get_runtime_attribute_by_name(attribute_base->get_attribute_name());

			ERR_FAIL_NULL_MSG(attribute, "Attribute not found in attribute set.");

			_affected_runtime_attributes.push_back(attribute);
			buffed_values.push_back(attribute->get_buffed_value());
		}

		TypedArray<AttributeOperation> operations;

		const bool applied = GDVIRTUAL_CALL_PTR(p_buff, _operate, buffed_values, attribute_set, operations);

		ERR_FAIL_COND_MSG(!applied, "An error occurred calling the overridden _operate method.");

		/// we are going to create a new AttributeBuff for each derived attribute affected by the buff
		/// we will add this buff to each affected runtime attribute.
		for (int i = 0; i < operations.size(); i++) {
			const Ref<AttributeOperation> attribute_operation = operations[i];
			const Ref<RuntimeAttribute> &runtime_attribute = _affected_runtime_attributes[i];

			ERR_FAIL_COND_MSG(!runtime_attribute.is_valid(), "Attribute not valid at index " + itos(i));

			String attribute_name = runtime_attribute->get_attribute()->get_attribute_name();
			const String change_set_name = attribute_name + "/" + p_buff->get_buff_name();
			Ref<AttributeChangeSet> attribute_change_set = buff_context->new_changeset(change_set_name);

			if (p_buff->get_unique() && buff_context->has_changeset(change_set_name)) {
				continue;
			}

			if (const int max_stack_size = p_buff->get_stack_size(); max_stack_size > 0 && buff_context->get_committed_changesets_by_name(change_set_name).size() >= max_stack_size) {
				continue;
			}

			const Ref<AttributeChangeSetOperation> attribute_changeset_operation = attribute_change_set->operate(attribute_name, attribute_operation.ptr());

			attribute_changeset_operation->set_duration(p_buff->get_duration(), manual_ticking ? AttributeChangeSetOperation::TICK_MANUAL : AttributeChangeSetOperation::TICK_MILLISECOND);
			attribute_changeset_operation->set_execution_order(p_buff->get_queue_execution());
			attribute_changeset_operation->set_transient(p_buff->get_transient());

			switch (p_buff->get_duration_merging()) {
				case AttributeBuff::DurationMerging::DURATION_MERGE_ADD: {
					TypedArray<AttributeChangeSet> other_changesets = buff_context->get_committed_changesets_by_name(change_set_name);

					for (int j = 0; j < other_changesets.size(); j++) {
						const Ref<AttributeChangeSet> &other_changeset = other_changesets[j];
						TypedArray<AttributeChangeSetOperation> other_operation = other_changeset->get_operations();

						for (int k = 0; k < other_operation.size(); k++) {
							const Ref<AttributeChangeSetOperation> &other_operation_ref = other_operation[k];
							other_operation_ref->set_remaining_duration(other_operation_ref->get_duration() + p_buff->get_duration());
						}
					}
				}
				break;
				case AttributeBuff::DurationMerging::DURATION_MERGE_RESTART:
					buff_context->rollback(change_set_name);
					break;
				case AttributeBuff::DurationMerging::DURATION_MERGE_STACK:
				default:
					break;
			}

			buff_context->commit(attribute_change_set);
		}
	} else {
		p_buff->apply(buff_context.ptr());
	}
}

void AttributeContainer::remove_attribute(const Ref<AttributeBase> &p_attribute)
{
	ERR_FAIL_NULL_MSG(p_attribute, "Attribute cannot be null, it must be an instance of a class inheriting from AttributeBase abstract class.");
	ERR_FAIL_COND_MSG(!has_attribute(p_attribute), "Attribute not found in the container.");

	const Ref<RuntimeAttribute> runtime_attribute = get_runtime_attribute_by_name(p_attribute->get_name());

	ERR_FAIL_COND_MSG(!runtime_attribute.is_valid(), "Attribute not valid.");

	const String attribute_name = runtime_attribute->get_attribute()->get_attribute_name();

	ERR_FAIL_COND_MSG(!attributes.has(attribute_name), "Attribute not found. This is a bug, please open an issue.");

	runtime_attribute->disconnect("attribute_changed", Callable::create(this, "_on_attribute_changed"));
	runtime_attribute->disconnect("buff_added", Callable::create(this, "_on_buff_applied"));
	runtime_attribute->disconnect("buff_removed", Callable::create(this, "_on_buff_removed"));

	ERR_FAIL_COND_MSG(!attributes.erase(attribute_name), "Failed to remove attribute from container.");
}

void AttributeContainer::remove_buff(const Ref<AttributeBuff> &p_buff) const
{
	ERR_FAIL_NULL_MSG(p_buff, "Buff cannot be null, it must be an instance of a class inheriting from AttributeBuff abstract class.");
	ERR_FAIL_COND_MSG(p_buff.is_null(), "Buff cannot be null, it must be an instance of a class inheriting from AttributeBuff abstract class.");

	buff_context->rollback(p_buff->get_buff_name());
}

void AttributeContainer::setup()
{
	attributes.clear();

	if (attribute_set.is_valid()) {
		for (int i = 0; i < attribute_set->count(); i++) {
			add_attribute(attribute_set->get_at(i));
		}
	}

	buff_context->set_attribute_container(this);
}

Ref<RuntimeAttribute> AttributeContainer::find(const Callable &p_predicate) const
{
	Array _attributes = attributes.values();

	for (int i = 0; i < _attributes.size(); i++) {
		if (p_predicate.call(_attributes[i])) {
			return _attributes[i];
		}
	}

	return nullptr;
}

float AttributeContainer::find_buffed_value(const Callable &p_predicate) const
{
	const Ref<RuntimeAttribute> attribute = find(p_predicate);
	return attribute.is_valid() ? attribute->get_buffed_value() : 0.0f;
}

float AttributeContainer::find_value(const Callable &p_predicate) const
{
	const Ref<RuntimeAttribute> attribute = find(p_predicate);
	return attribute.is_valid() ? attribute->get_value() : 0.0f;
}

Ref<AttributeSet> AttributeContainer::get_attribute_set() const
{
	return attribute_set;
}

bool AttributeContainer::get_manual_ticking() const
{
	return manual_ticking;
}

TypedArray<RuntimeAttribute> AttributeContainer::get_runtime_attributes() const
{
	return attributes.values();
}

Ref<RuntimeAttribute> AttributeContainer::get_runtime_attribute_by_name(const String &p_name) const
{
	if (attributes.has(p_name)) {
		return attributes[p_name];
	}

	return {};
}

float AttributeContainer::get_attribute_buffed_value_by_name(const String &p_name) const
{
	const Ref<RuntimeAttribute> attribute = get_runtime_attribute_by_name(p_name);
	return attribute.is_valid() ? attribute->get_buffed_value() : 0.0f;
}

float AttributeContainer::get_attribute_previous_value_by_name(const String &p_name) const
{
	const Ref<RuntimeAttribute> attribute = get_runtime_attribute_by_name(p_name);
	return attribute.is_valid() ? attribute->get_previous_value() : 0.0f;
}

float AttributeContainer::get_attribute_value_by_name(const String &p_name) const
{
	const Ref<RuntimeAttribute> attribute = get_runtime_attribute_by_name(p_name);
	return attribute.is_valid() ? attribute->get_value() : 0.0f;
}

void AttributeContainer::rollback_change_set(const String &p_changeset_name) const
{
	buff_context->rollback(p_changeset_name);
}

void AttributeContainer::set_attribute_set(const Ref<AttributeSet> &p_attribute_set)
{
	attribute_set = p_attribute_set;
	setup();
}

void AttributeContainer::set_manual_ticking(const bool p_manual_ticking)
{
	manual_ticking = p_manual_ticking;
}

void AttributeContainer::subtract_attribute_buffs_ticks(const float p_tick) const
{
	buff_context->tick_operations(p_tick);
}

void AttributeContainer::_bind_methods()
{
	/// binds methods to godot
	ClassDB::bind_method(D_METHOD("_on_attribute_changed", "p_attribute", "p_previous_value", "p_new_value"), &AttributeContainer::_on_attribute_changed);
	ClassDB::bind_method(D_METHOD("_on_buff_applied", "p_buff"), &AttributeContainer::_on_buff_applied);
	ClassDB::bind_method(D_METHOD("_on_buff_removed", "p_buff"), &AttributeContainer::_on_buff_removed);
	ClassDB::bind_method(D_METHOD("_on_buff_time_updated", "p_buff"), &AttributeContainer::_on_buff_time_updated);
	ClassDB::bind_method(D_METHOD("add_attribute", "p_attribute"), &AttributeContainer::add_attribute);
	ClassDB::bind_method(D_METHOD("apply_buff", "p_buff"), &AttributeContainer::apply_buff);
	ClassDB::bind_method(D_METHOD("find", "p_predicate"), &AttributeContainer::find);
	ClassDB::bind_method(D_METHOD("find_buffed_value", "p_predicate"), &AttributeContainer::find_buffed_value);
	ClassDB::bind_method(D_METHOD("find_value", "p_predicate"), &AttributeContainer::find_value);
	ClassDB::bind_method(D_METHOD("get_attribute_set"), &AttributeContainer::get_attribute_set);
	ClassDB::bind_method(D_METHOD("get_attributes"), &AttributeContainer::get_runtime_attributes);
	ClassDB::bind_method(D_METHOD("get_attribute_by_name", "p_name"), &AttributeContainer::get_runtime_attribute_by_name);
	ClassDB::bind_method(D_METHOD("get_attribute_buffed_value_by_name", "p_name"), &AttributeContainer::get_attribute_buffed_value_by_name);
	ClassDB::bind_method(D_METHOD("get_attribute_value_by_name", "p_name"), &AttributeContainer::get_attribute_value_by_name);
	ClassDB::bind_method(D_METHOD("get_manual_ticking"), &AttributeContainer::get_manual_ticking);
	ClassDB::bind_method(D_METHOD("remove_attribute", "p_attribute"), &AttributeContainer::remove_attribute);
	ClassDB::bind_method(D_METHOD("remove_buff", "p_buff"), &AttributeContainer::remove_buff);
	ClassDB::bind_method(D_METHOD("rollback_change_set", "p_changeset_name"), &AttributeContainer::rollback_change_set);
	ClassDB::bind_method(D_METHOD("set_attribute_set", "p_attribute_set"), &AttributeContainer::set_attribute_set);
	ClassDB::bind_method(D_METHOD("setup"), &AttributeContainer::setup);
	ClassDB::bind_method(D_METHOD("set_manual_ticking", "p_manual_ticking"), &AttributeContainer::set_manual_ticking);
	ClassDB::bind_method(D_METHOD("subtract_attribute_buffs_ticks", "p_tick"), &AttributeContainer::subtract_attribute_buffs_ticks);

	/// binds properties to godot
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "attribute_set", PROPERTY_HINT_RESOURCE_TYPE, "AttributeSet"), "set_attribute_set", "get_attribute_set");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "manual_ticking"), "set_manual_ticking", "get_manual_ticking");

	/// signals binding
	ADD_SIGNAL(MethodInfo("attribute_changed", PropertyInfo(Variant::OBJECT, "attribute", PROPERTY_HINT_RESOURCE_TYPE, "RuntimeAttributeBase"), PropertyInfo(Variant::FLOAT, "previous_value"), PropertyInfo(Variant::FLOAT, "new_value")));
	ADD_SIGNAL(MethodInfo("buff_applied", PropertyInfo(Variant::OBJECT, "buff", PROPERTY_HINT_RESOURCE_TYPE, "RuntimeBuff")));
	ADD_SIGNAL(MethodInfo("buff_dequeued", PropertyInfo(Variant::OBJECT, "buff", PROPERTY_HINT_RESOURCE_TYPE, "RuntimeBuff")));
	ADD_SIGNAL(MethodInfo("buff_enqueued", PropertyInfo(Variant::OBJECT, "buff", PROPERTY_HINT_RESOURCE_TYPE, "RuntimeBuff")));
	ADD_SIGNAL(MethodInfo("buff_removed", PropertyInfo(Variant::OBJECT, "buff", PROPERTY_HINT_RESOURCE_TYPE, "RuntimeBuff")));
	ADD_SIGNAL(MethodInfo("buff_time_elapsed", PropertyInfo(Variant::OBJECT, "buff", PROPERTY_HINT_RESOURCE_TYPE, "RuntimeBuff")));
	ADD_SIGNAL(MethodInfo("buff_time_updated", PropertyInfo(Variant::OBJECT, "buff", PROPERTY_HINT_RESOURCE_TYPE, "RuntimeBuff")));
}