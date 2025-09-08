/**************************************************************************/
/*  attribute.cpp                                                         */
/**************************************************************************/
/*                         This file is part of:                          */
/*                        Godot Gameplay Systems                          */
/*              https://github.com/OctoD/godot-gameplay-systems           */
/**************************************************************************/
/* Read the license file in this repo.						              */
/**************************************************************************/

#include "attribute.hpp"
#include "attribute_container.hpp"

using namespace octod::gameplay::attributes;

#pragma region AttributeOperation

void AttributeOperation::_bind_methods()
{
	/// binds methods to godot
	ClassDB::bind_method(D_METHOD("get_operand"), &AttributeOperation::get_operand);
	ClassDB::bind_method(D_METHOD("get_value"), &AttributeOperation::get_value);
	ClassDB::bind_method(D_METHOD("operate", "base_value"), &AttributeOperation::operate);
	ClassDB::bind_method(D_METHOD("set_operand", "p_value"), &AttributeOperation::set_operand);
	ClassDB::bind_method(D_METHOD("set_value", "p_value"), &AttributeOperation::set_value);

	/// binds static methods to godot
	ClassDB::bind_static_method("AttributeOperation", D_METHOD("add", "p_value"), &AttributeOperation::add);
	ClassDB::bind_static_method("AttributeOperation", D_METHOD("divide", "p_value"), &AttributeOperation::divide);
	ClassDB::bind_static_method("AttributeOperation", D_METHOD("multiply", "p_value"), &AttributeOperation::multiply);
	ClassDB::bind_static_method("AttributeOperation", D_METHOD("percentage", "p_value"), &AttributeOperation::percentage);
	ClassDB::bind_static_method("AttributeOperation", D_METHOD("subtract", "p_value"), &AttributeOperation::subtract);
	ClassDB::bind_static_method("AttributeOperation", D_METHOD("forcefully_set_value", "p_value"), &AttributeOperation::forcefully_set_value);

	/// binds properties
	ADD_PROPERTY(PropertyInfo(Variant::INT, "operand", PROPERTY_HINT_ENUM, "Add:0,Divide:1,Multiply:2,Percentage:3,Subtract:4,Set:5"), "set_operand", "get_operand");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "value"), "set_value", "get_value");

	/// binds enum as consts
	BIND_ENUM_CONSTANT(OP_ADD);
	BIND_ENUM_CONSTANT(OP_DIVIDE);
	BIND_ENUM_CONSTANT(OP_MULTIPLY);
	BIND_ENUM_CONSTANT(OP_PERCENTAGE);
	BIND_ENUM_CONSTANT(OP_SUBTRACT);
	BIND_ENUM_CONSTANT(OP_SET);
}

Ref<AttributeOperation> AttributeOperation::create(const OperationType p_operand, const float p_value)
{
	Ref operation = memnew(AttributeOperation);
	operation->set_operand(p_operand);
	operation->set_value(p_value);
	return operation;
}

Ref<AttributeOperation> AttributeOperation::add(const float p_value)
{
	return create(OP_ADD, p_value);
}

Ref<AttributeOperation> AttributeOperation::divide(const float p_value)
{
	return create(OP_DIVIDE, p_value);
}

Ref<AttributeOperation> AttributeOperation::multiply(const float p_value)
{
	return create(OP_MULTIPLY, p_value);
}

Ref<AttributeOperation> AttributeOperation::percentage(const float p_value)
{
	return create(OP_PERCENTAGE, p_value);
}

Ref<AttributeOperation> AttributeOperation::subtract(const float p_value)
{
	return create(OP_SUBTRACT, p_value);
}

Ref<AttributeOperation> AttributeOperation::forcefully_set_value(const float p_value)
{
	return create(OP_SET, p_value);
}

bool AttributeOperation::equals_to(const Ref<AttributeOperation> &other) const
{
	if (other == nullptr) {
		return false;
	}

	return operand == other->operand && Math::is_equal_approx(value, other->value);
}

int AttributeOperation::get_operand() const
{
	return operand;
}

float AttributeOperation::get_value() const
{
	return value;
}

float AttributeOperation::operate(float p_base_value) const
{
	switch (operand) {
		case OP_ADD:
			return p_base_value + value;
		case OP_DIVIDE:
			return Math::absf(value) < 0.0001f ? 0 : p_base_value / value;
		case OP_MULTIPLY:
			return p_base_value * value;
		case OP_PERCENTAGE:
			return p_base_value + ((p_base_value / 100) * value);
		case OP_SUBTRACT:
			return p_base_value - value;
		case OP_SET:
			return value;
		default:
			return p_base_value;
	}
}

void AttributeOperation::set_operand(const int p_value)
{
	operand = static_cast<OperationType>(p_value);
}

void AttributeOperation::set_value(const float p_value)
{
	value = p_value;
}

#pragma endregion

#pragma region AttributeBuffBase

void AttributeBuffBase::_bind_methods()
{
	/// binds virtuals to godot
	GDVIRTUAL_BIND(_applies_to, "attribute_set");
	GDVIRTUAL_BIND(_operate, "values", "attribute_set");
}

#pragma endregion

#pragma region AttributeBuff

void AttributeBuff::_bind_methods()
{
	/// binds methods to godot
	ClassDB::bind_method(D_METHOD("get_attribute_name"), &AttributeBuff::get_attribute_name);
	ClassDB::bind_method(D_METHOD("get_buff_name"), &AttributeBuff::get_buff_name);
	ClassDB::bind_method(D_METHOD("get_duration"), &AttributeBuff::get_duration);
	ClassDB::bind_method(D_METHOD("get_duration_merging"), &AttributeBuff::get_duration_merging);
	ClassDB::bind_method(D_METHOD("get_max_applies"), &AttributeBuff::get_stack_size);
	ClassDB::bind_method(D_METHOD("get_operation"), &AttributeBuff::get_operation);
	ClassDB::bind_method(D_METHOD("get_parent_buff"), &AttributeBuff::get_parent_buff);
	ClassDB::bind_method(D_METHOD("get_queue_execution"), &AttributeBuff::get_queue_execution);
	ClassDB::bind_method(D_METHOD("get_stack_size"), &AttributeBuff::get_stack_size);
	ClassDB::bind_method(D_METHOD("get_transient"), &AttributeBuff::get_transient);
	ClassDB::bind_method(D_METHOD("get_unique"), &AttributeBuff::get_unique);
	ClassDB::bind_method(D_METHOD("operate", "base_value"), &AttributeBuff::operate);
	ClassDB::bind_method(D_METHOD("set_attribute_name", "p_value"), &AttributeBuff::set_attribute_name);
	ClassDB::bind_method(D_METHOD("set_buff_name", "p_value"), &AttributeBuff::set_buff_name);
	ClassDB::bind_method(D_METHOD("set_duration", "p_value"), &AttributeBuff::set_duration);
	ClassDB::bind_method(D_METHOD("set_duration_merging", "p_value"), &AttributeBuff::set_duration_merging);
	ClassDB::bind_method(D_METHOD("set_max_applies", "p_value"), &AttributeBuff::set_stack_size);
	ClassDB::bind_method(D_METHOD("set_operation", "p_value"), &AttributeBuff::set_operation);
	ClassDB::bind_method(D_METHOD("set_parent_buff", "p_value"), &AttributeBuff::set_parent_buff);
	ClassDB::bind_method(D_METHOD("set_queue_execution", "p_value"), &AttributeBuff::set_queue_execution);
	ClassDB::bind_method(D_METHOD("set_stack_size", "p_value"), &AttributeBuff::set_stack_size);
	ClassDB::bind_method(D_METHOD("set_transient", "p_value"), &AttributeBuff::set_transient);
	ClassDB::bind_method(D_METHOD("set_unique", "p_value"), &AttributeBuff::set_unique);

	/// binds properties to godot
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "attribute_name"), "set_attribute_name", "get_attribute_name");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "buff_name"), "set_buff_name", "get_buff_name");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "duration"), "set_duration", "get_duration");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "duration_merging", PROPERTY_HINT_ENUM, "Add:0,Stack:1,Restart:2"), "set_duration_merging", "get_duration_merging");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "operation", PROPERTY_HINT_RESOURCE_TYPE, "AttributeOperation"), "set_operation", "get_operation");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "parent_buff", PROPERTY_HINT_RESOURCE_TYPE, "AttributeBuff"), "set_parent_buff", "get_parent_buff");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "max_applies"), "set_stack_size", "get_stack_size");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "stack_size"), "set_stack_size", "get_stack_size");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "queue_execution", PROPERTY_HINT_ENUM, "Parallel:0,Waterfall:1"), "set_queue_execution", "get_queue_execution");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "transient"), "set_transient", "get_transient");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "unique"), "set_unique", "get_unique");

	/// binds enum constants
	BIND_ENUM_CONSTANT(DURATION_MERGE_ADD);
	BIND_ENUM_CONSTANT(DURATION_MERGE_STACK);
	BIND_ENUM_CONSTANT(DURATION_MERGE_RESTART);
	BIND_ENUM_CONSTANT(QUEUE_EXECUTION_PARALLEL)
	BIND_ENUM_CONSTANT(QUEUE_EXECUTION_WATERFALL)
}

void AttributeBuff::apply(AttributeBuffContext *context)
{
	ERR_FAIL_NULL_MSG(context, "Cannot apply AttributeBuff, AttributeBuffContext is null.");

	if (GDVIRTUAL_IS_OVERRIDDEN(_apply)) {
		GDVIRTUAL_CALL(_apply, context);
		return;
	}

	if (!context->has_attribute(attribute_name)) {
		return;
	}

	const Ref changeset = context->new_changeset(buff_name);

	changeset->operate(attribute_name, operation.ptr())->set_duration(duration);

	context->commit(changeset);
}

bool AttributeBuff::is_apply_overridden() const
{
	return GDVIRTUAL_IS_OVERRIDDEN(_apply);
}

bool AttributeBuff::equals_to(const Ref<AttributeBuff> &buff) const
{
	if (buff == nullptr) {
		return false;
	}

	ERR_FAIL_COND_V_MSG(buff.is_null(), false, "Cannot compare to null AttributeBuff. This is a bug, please report it.");

	return (
		Math::is_equal_approx(buff->duration, duration) && attribute_name == buff->attribute_name && buff_name == buff->buff_name && duration_merging == buff->duration_merging && max_stacking == buff->max_stacking && queue_execution == buff->queue_execution && transient == buff->transient && unique == buff->unique);
}

float AttributeBuff::operate(const float base_value) const
{
	ERR_FAIL_COND_V_MSG(operation.is_null(), 0.0f, "AttributeBuff operation is null, cannot operate on base value.");
	return operation->operate(base_value);
}

String AttributeBuff::get_attribute_name() const
{
	return attribute_name;
}

String AttributeBuff::get_buff_name() const
{
	return buff_name;
}

float AttributeBuff::get_duration() const
{
	return duration;
}

int AttributeBuff::get_duration_merging() const
{
	return duration_merging;
}

bool AttributeBuff::get_transient() const
{
	if (Math::is_zero_approx(duration)) {
		return transient;
	}

	return !Math::is_equal_approx(duration, 0.05f);
}

bool AttributeBuff::get_unique() const
{
	return unique;
}

bool AttributeBuff::is_operate_overridden() const
{
	return GDVIRTUAL_IS_OVERRIDDEN_PTR(this, _operate);
}

Ref<AttributeOperation> AttributeBuff::get_operation() const
{
	return operation;
}

Ref<AttributeBuff> AttributeBuff::get_parent_buff() const
{
	return parent_buff;
}

int AttributeBuff::get_stack_size() const
{
	return max_stacking;
}

int AttributeBuff::get_queue_execution() const
{
	return queue_execution;
}

bool AttributeBuff::is_time_limited() const
{
	return Math::absf(1.0f - duration) > 0.0001f;
}

void AttributeBuff::set_attribute_name(const String &p_value)
{
	attribute_name = p_value;
}

void AttributeBuff::set_buff_name(const String &p_value)
{
	buff_name = p_value;
}

void AttributeBuff::set_duration(const float p_value)
{
	duration = p_value;
}

void AttributeBuff::set_duration_merging(int p_value)
{
	duration_merging = static_cast<DurationMerging>(p_value);
}

void AttributeBuff::set_operation(const Ref<AttributeOperation> &p_value)
{
	operation = p_value;
}

void AttributeBuff::set_parent_buff(const Ref<AttributeBuff> &p_value)
{
	parent_buff = p_value;
}

void AttributeBuff::set_stack_size(const int p_value)
{
	max_stacking = p_value;
}

void AttributeBuff::set_queue_execution(int p_value)
{
	queue_execution = static_cast<QueueExecution>(p_value);
}

void AttributeBuff::set_transient(const bool p_value)
{
	transient = p_value;
}

void AttributeBuff::set_unique(const bool p_value)
{
	unique = p_value;
}

#pragma endregion

#pragma region AttributeComputationArgument

AttributeContainer *AttributeComputationArgument::get_attribute_container() const
{
	return attribute_container;
}

AttributeBuff *AttributeComputationArgument::get_buff() const
{
	return buff;
}

float AttributeComputationArgument::get_operated_value() const
{
	return operated_value;
}

TypedArray<RuntimeAttribute> AttributeComputationArgument::get_parent_attributes() const
{
	if (runtime_attribute) {
		return runtime_attribute->get_parent_runtime_attributes();
	}

	return {};
}

RuntimeAttribute *AttributeComputationArgument::get_runtime_attribute() const
{
	return runtime_attribute;
}

void AttributeComputationArgument::set_buff(AttributeBuff *p_buff)
{
	buff = p_buff;
}

void AttributeComputationArgument::set_operated_value(const float &p_value)
{
	operated_value = p_value;
}

void AttributeComputationArgument::set_runtime_attribute(RuntimeAttribute *p_runtime_attribute)
{
	runtime_attribute = p_runtime_attribute;
}

void AttributeComputationArgument::set_attribute_container(AttributeContainer *p_attribute_container)
{
	attribute_container = p_attribute_container;
}

void AttributeComputationArgument::_bind_methods()
{
	/// binds methods to godot
	ClassDB::bind_method(D_METHOD("get_attribute_container"), &AttributeComputationArgument::get_attribute_container);
	ClassDB::bind_method(D_METHOD("get_buff"), &AttributeComputationArgument::get_buff);
	ClassDB::bind_method(D_METHOD("get_operated_value"), &AttributeComputationArgument::get_operated_value);
	ClassDB::bind_method(D_METHOD("get_parent_attributes"), &AttributeComputationArgument::get_parent_attributes);
	ClassDB::bind_method(D_METHOD("get_runtime_attribute"), &AttributeComputationArgument::get_runtime_attribute);
	ClassDB::bind_method(D_METHOD("set_buff", "p_buff"), &AttributeComputationArgument::set_buff);
	ClassDB::bind_method(D_METHOD("set_operated_value", "p_value"), &AttributeComputationArgument::set_operated_value);
	ClassDB::bind_method(D_METHOD("set_runtime_attribute", "p_runtime_attribute"), &AttributeComputationArgument::set_runtime_attribute);

	/// binds properties to godot
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "buff", PROPERTY_HINT_RESOURCE_TYPE, "24:17/AttributeBuff"), "set_buff", "get_buff");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "operated_value"), "set_operated_value", "get_operated_value");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "runtime_attribute", PROPERTY_HINT_RESOURCE_TYPE, "RuntimeAttribute"), "set_runtime_attribute", "get_runtime_attribute");
}

#pragma endregion

#pragma region AttributeBase

void AttributeBase::_bind_methods()
{
	/// binds methods to godot
	ClassDB::bind_method(D_METHOD("get_attribute_name"), &AttributeBase::get_attribute_name);
	ClassDB::bind_method(D_METHOD("set_attribute_name", "p_value"), &AttributeBase::set_attribute_name);

	/// binds virtuals to godot
	GDVIRTUAL_BIND(_derived_from, "attribute_set");
	GDVIRTUAL_BIND(_compute_value, "attribute_computation");

	/// binds properties to godot
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "attribute_name"), "set_attribute_name", "get_attribute_name");
}

String AttributeBase::get_attribute_name() const
{
	if (attribute_name.is_empty()) {
		return get_class_static();
	}

	return attribute_name;
}

void AttributeBase::set_attribute_name(const String &p_value)
{
	attribute_name = p_value;
}

#pragma endregion

#pragma region Attribute

void Attribute::_bind_methods()
{
}

#pragma endregion

#pragma region AttributeSet

void AttributeSet::_bind_methods()
{
	/// binds methods to godot
	ClassDB::bind_method(D_METHOD("add_attribute", "p_attribute"), &AttributeSet::add_attribute);
	ClassDB::bind_method(D_METHOD("add_attributes", "p_attributes"), &AttributeSet::add_attributes);
	ClassDB::bind_method(D_METHOD("find_by_classname", "p_classname"), &AttributeSet::find_by_classname);
	ClassDB::bind_method(D_METHOD("find_by_name", "p_name"), &AttributeSet::find_by_name);
	ClassDB::bind_method(D_METHOD("get_attributes_names"), &AttributeSet::get_attributes_names);
	ClassDB::bind_method(D_METHOD("get_attributes"), &AttributeSet::get_attributes);
	ClassDB::bind_method(D_METHOD("get_set_name"), &AttributeSet::get_set_name);
	ClassDB::bind_method(D_METHOD("has_attribute", "p_attribute"), &AttributeSet::has_attribute);
	ClassDB::bind_method(D_METHOD("remove_attribute", "p_attribute"), &AttributeSet::remove_attribute);
	ClassDB::bind_method(D_METHOD("remove_attributes", "p_attributes"), &AttributeSet::remove_attributes);
	ClassDB::bind_method(D_METHOD("set_attributes", "p_attributes"), &AttributeSet::set_attributes);
	ClassDB::bind_method(D_METHOD("set_set_name", "p_value"), &AttributeSet::set_set_name);

	/// binds properties to godot
	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "attributes", PROPERTY_HINT_RESOURCE_TYPE, "24/17:AttributeBase"), "set_attributes", "get_attributes");
	ADD_PROPERTY(PropertyInfo(Variant::STRING, "set_name"), "set_set_name", "get_set_name");

	/// adds signals to godot
	ADD_SIGNAL(MethodInfo("attribute_added", PropertyInfo(Variant::OBJECT, "attribute", PROPERTY_HINT_RESOURCE_TYPE, "AttributeBase")));
	ADD_SIGNAL(MethodInfo("attribute_removed", PropertyInfo(Variant::OBJECT, "attribute", PROPERTY_HINT_RESOURCE_TYPE, "AttributeBase")));
}

AttributeSet::AttributeSet()
{
	attributes = TypedArray<AttributeBase>();
}

AttributeSet::AttributeSet(const TypedArray<AttributeBase> &p_attributes, const String &p_set_name)
{
	attributes = p_attributes.duplicate(true);
	set_name = p_set_name;
}

bool AttributeSet::add_attribute(const Ref<AttributeBase> &p_attribute)
{
	if (!has_attribute(p_attribute)) {
		const Ref<Attribute> d_attribute = p_attribute->duplicate(true);

		attributes.push_back(d_attribute);
		emit_signal("attribute_added", d_attribute);
		emit_changed();
		return true;
	}

	return false;
}

uint16_t AttributeSet::add_attributes(const TypedArray<AttributeBase> &p_attributes)
{
	uint16_t count = 0;

	for (int i = 0; i < p_attributes.size(); i++) {
		if (!has_attribute(p_attributes[i])) {
			Ref<AttributeBase> d_attribute = p_attributes[i];

			d_attribute = d_attribute->duplicate(true);

			attributes.push_back(d_attribute);
			count++;
			emit_signal("attribute_added", d_attribute);
		}
	}

	if (count > 0) {
		emit_changed();
	}

	return count;
}

int AttributeSet::find(const Ref<AttributeBase> &p_attribute) const
{
	return static_cast<int>(attributes.find(p_attribute));
}

Ref<AttributeBase> AttributeSet::find_by_classname(const String &p_classname) const
{
	for (int i = 0; i < attributes.size(); i++) {
		if (const Ref<AttributeBase> attribute = attributes[i]; attribute->get_class() == p_classname) {
			return attributes[i];
		}
	}

	return Ref<Attribute>();
}

Ref<AttributeBase> AttributeSet::find_by_name(const String &p_name) const
{
	for (int i = 0; i < attributes.size(); i++) {
		if (const Ref<AttributeBase> attribute = attributes[i]; attribute->get_attribute_name() == p_name) {
			return attributes[i];
		}
	}

	return {};
}

PackedStringArray AttributeSet::get_attributes_names() const
{
	PackedStringArray names;

	for (int i = 0; i < attributes.size(); i++) {
		const Ref<AttributeBase> attribute = attributes[i];
		names.push_back(attribute->get_attribute_name());
	}

	return names;
}

TypedArray<AttributeBase> AttributeSet::get_attributes() const
{
	return attributes;
}

Ref<AttributeBase> AttributeSet::get_at(const int index) const
{
	if (index >= 0 && index < attributes.size()) {
		return attributes[index];
	}

	return {};
}

String AttributeSet::get_set_name() const
{
	return set_name;
}

bool AttributeSet::has_attribute(const Ref<AttributeBase> &p_attribute) const
{
	for (int i = 0; i < attributes.size(); i++) {
		if (attributes[i] == p_attribute) {
			return true;
		}
	}

	return false;
}

bool AttributeSet::remove_attribute(const Ref<AttributeBase> &p_attribute)
{
	if (const int64_t index = attributes.find(p_attribute); index != -1) {
		attributes.remove_at(index);
		emit_signal("attribute_removed", p_attribute);
		emit_changed();
		return true;
	}

	return false;
}

int AttributeSet::remove_attributes(const TypedArray<AttributeBase> &p_attributes)
{
	int count = 0;

	for (int i = 0; i < p_attributes.size(); i++) {
		if (const int64_t index = attributes.find(p_attributes[i]); index != -1) {
			attributes.remove_at(index);
			count++;
			emit_signal("attribute_removed", p_attributes[i]);
		}
	}

	if (count > 0) {
		emit_changed();
	}

	return count;
}

void AttributeSet::push_back(const Ref<AttributeBase> &p_attribute)
{
	attributes.push_back(p_attribute);
	emit_signal("attribute_added", p_attribute);
	emit_changed();
}

void AttributeSet::set_attributes(const TypedArray<AttributeBase> &p_attributes)
{
	attributes = p_attributes;
	emit_changed();
}

void AttributeSet::set_set_name(const String &p_value)
{
	set_name = p_value;
	emit_changed();
}

int AttributeSet::count() const
{
	return static_cast<int>(attributes.size());
}

#pragma endregion

#pragma region RuntimeBuff

void RuntimeBuff::_bind_methods()
{
	/// binds methods to godot
	ClassDB::bind_method(D_METHOD("get_attribute_name"), &RuntimeBuff::get_attribute_name);
	ClassDB::bind_method(D_METHOD("get_buff_name"), &RuntimeBuff::get_buff_name);
	ClassDB::bind_method(D_METHOD("get_duration"), &RuntimeBuff::get_duration);
	ClassDB::bind_method(D_METHOD("get_time_left"), &RuntimeBuff::get_time_left);
	ClassDB::bind_method(D_METHOD("set_time_left", "p_value"), &RuntimeBuff::set_time_left);
	ClassDB::bind_method(D_METHOD("get_buff"), &RuntimeBuff::get_buff);
	ClassDB::bind_method(D_METHOD("set_buff", "p_value"), &RuntimeBuff::set_buff);

	/// binds properties to godot
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "buff", PROPERTY_HINT_RESOURCE_TYPE, "AttributeBuff"), "set_buff", "get_buff");
}

bool RuntimeBuff::equals_to(const Ref<AttributeBuff> &p_buff) const
{
	return buff->equals_to(p_buff);
}

Ref<RuntimeAttribute> RuntimeBuff::applies_to(const AttributeContainer *p_attribute_container) const
{
	Ref<RuntimeAttribute> attribute = p_attribute_container->get_runtime_attribute_by_name(buff->get_attribute_name());

	ERR_FAIL_COND_V_MSG(attribute.is_null(), attribute, "Attribute not found in attribute set.");
	ERR_FAIL_COND_V_MSG(!attribute.is_valid(), attribute, "Attribute reference is not valid.");

	return attribute;
}

float RuntimeBuff::operate(const Ref<RuntimeAttribute> &p_runtime_attribute) const
{
	ERR_FAIL_COND_V_MSG(!buff.is_valid(), 0.0f, "Buff is not valid, cannot operate on runtime attributes.");
	ERR_FAIL_NULL_V_MSG(p_runtime_attribute, 0.0f, "Runtime attribute is null, cannot operate on it.");

	return buff->operate(p_runtime_attribute->value);
}

bool RuntimeBuff::can_apply_to_attribute(const Ref<RuntimeAttribute> &p_attribute) const
{
	const Ref<RuntimeAttribute> applied_to = applies_to(p_attribute->attribute_container);

	ERR_FAIL_NULL_V_MSG(applied_to, false, "Attribute not found in attribute set.");
	ERR_FAIL_COND_V_MSG(!applied_to.is_valid(), false, "Attribute reference is not valid.");

	return applied_to->attribute->get_attribute_name() == p_attribute->attribute->get_attribute_name();
}

bool RuntimeBuff::can_dequeue() const
{
	return Math::is_zero_approx(time_left);
}

String RuntimeBuff::get_attribute_name() const
{
	return buff->attribute_name;
}

String RuntimeBuff::get_buff_name() const
{
	return buff->buff_name;
}

Ref<AttributeBuff> RuntimeBuff::get_buff() const
{
	return buff;
}

float RuntimeBuff::get_duration() const
{
	return buff->duration;
}

float RuntimeBuff::get_time_left() const
{
	return time_left;
}

bool RuntimeBuff::has_duration() const
{
	return !Math::is_zero_approx(buff->duration);
}

bool RuntimeBuff::is_operate_overridden() const
{
	return GDVIRTUAL_IS_OVERRIDDEN_PTR(buff, _operate);
}

bool RuntimeBuff::is_transient() const
{
	ERR_FAIL_COND_V_MSG(!buff.is_valid(), false, "AttributeBuff is not valid.");
	return buff->transient;
}

bool RuntimeBuff::is_transient_time_based() const
{
	ERR_FAIL_COND_V_MSG(buff.is_null(), false, "AttributeBuff is null.");
	return buff->transient && !Math::is_zero_approx(buff->duration);
}

void RuntimeBuff::set_buff(const Ref<AttributeBuff> &p_value)
{
	buff = p_value;
}

void RuntimeBuff::set_time_left(const float p_value)
{
	time_left = Math::clamp(p_value, 0.0f, buff->get_duration());
}

#pragma endregion

#pragma region RuntimeAttributeBase

Ref<RuntimeBuff> RuntimeAttribute::add_buff(const Ref<AttributeBuff> &p_buff)
{
	Ref<RuntimeBuff> runtime_buff;

	if (!can_receive_buff(p_buff)) {
		return runtime_buff;
	}

	runtime_buff.instantiate();
	runtime_buff->buff = p_buff;
	runtime_buff->time_left = p_buff->get_duration();

	ERR_FAIL_COND_V_MSG(runtime_buff.is_null(), runtime_buff, "Failed to create runtime buff from attribute buff.");

	if (p_buff->get_transient()) {
		if (const auto duration_merging = runtime_buff->buff->get_duration_merging(); duration_merging == AttributeBuff::DURATION_MERGE_ADD || duration_merging == AttributeBuff::DURATION_MERGE_RESTART) {
			for (int i = 0; i < buffs.size(); i++) {
				if (const auto maybe_runtime_buff = cast_to<RuntimeBuff>(buffs[i]); maybe_runtime_buff && maybe_runtime_buff->buff->equals_to(p_buff)) {
					if (duration_merging == AttributeBuff::DURATION_MERGE_ADD) {
						maybe_runtime_buff->set_time_left(maybe_runtime_buff->get_time_left() + p_buff->get_duration());
					} else {
						maybe_runtime_buff->set_time_left(p_buff->get_duration());
					}

					emit_signal("buff_time_updated", maybe_runtime_buff);
					return maybe_runtime_buff;
				}
			}

			runtime_buff->set_time_left(p_buff->get_duration());
		}

		buffs.push_back(runtime_buff);
		emit_signal("buff_added", runtime_buff);

		if (!Math::is_zero_approx(p_buff->get_duration())) {
			emit_signal("buff_enqueued", runtime_buff);
		}
	} else {
		previous_value = value;

		if (GDVIRTUAL_IS_OVERRIDDEN_PTR(attribute, _compute_value)) {
			AttributeComputationArgument *argument = memnew(AttributeComputationArgument);

			argument->set_attribute_container(attribute_container);
			argument->set_buff(p_buff.ptr());
			argument->set_operated_value(runtime_buff->operate(this));
			argument->set_runtime_attribute(this);

			GDVIRTUAL_CALL_PTR(attribute, _compute_value, argument, value);
		} else if (runtime_buff->can_apply_to_attribute(this)) {
			value = runtime_buff->operate(this);
		}

		if (!Math::is_equal_approx(previous_value, value)) {
			emit_signal("attribute_changed", this, previous_value, value);
		}
	}

	return runtime_buff;
}

bool RuntimeAttribute::can_receive_buff(const Ref<AttributeBuff> &p_buff) const
{
	if (p_buff->get_unique() && has_buff(p_buff)) {
		return false;
	}

	int buffs_count = 0;

	for (int i = 0; i < buffs.size(); i++) {
		if (const Ref<RuntimeBuff> buff = buffs[i]; buff->equals_to(p_buff)) {
			buffs_count++;
		}
	}

	if (buffs_count >= p_buff->get_stack_size() && p_buff->get_stack_size() > 0) {
		return false;
	}

	return p_buff->get_attribute_name() == attribute->get_attribute_name();
}

void RuntimeAttribute::compute_value()
{
	if (GDVIRTUAL_IS_OVERRIDDEN_PTR(attribute, _compute_value)) {
		AttributeComputationArgument *argument = memnew(AttributeComputationArgument);

		const float _previous_value = value;

		argument->set_attribute_container(attribute_container);
		argument->set_buff(nullptr);
		argument->set_operated_value(value);
		argument->set_runtime_attribute(this);

		GDVIRTUAL_CALL_PTR(attribute, _compute_value, argument, value);

		if (!Math::is_equal_approx(_previous_value, value)) {
			previous_value = _previous_value;
			emit_signal("attribute_changed", this, previous_value, value);
		}
	}
}

void RuntimeAttribute::clear_buffs()
{
	buffs.clear();
}

String RuntimeAttribute::get_attribute_name() const
{
	ERR_FAIL_COND_V_MSG(attribute.is_null(), "", "Attribute is null, cannot get attribute name.");
	return attribute->get_attribute_name();
}

TypedArray<RuntimeAttribute> RuntimeAttribute::get_parent_runtime_attributes() const
{
	if (const auto derived = get_derived_from(); derived.size() > 0) {
		TypedArray<RuntimeAttribute> parent_attributes;

		for (int i = 0; i < derived.size(); i++) {
			if (Ref attribute_base = cast_to<AttributeBase>(derived[i]); attribute_base.is_valid()) {
				if (const auto parent_attribute = attribute_container->get_runtime_attribute_by_name(attribute_base->get_attribute_name()); parent_attribute.is_valid()) {
					parent_attributes.push_back(parent_attribute);
				}
			}
		}

		return parent_attributes;
	}

	return {};
}

bool RuntimeAttribute::has_buff(const Ref<AttributeBuff> &p_buff) const
{
	for (int i = 0; i < buffs.size(); i++) {
		if (cast_to<RuntimeBuff>(buffs[i])->equals_to(p_buff)) {
			return true;
		}
	}

	return false;
}

bool RuntimeAttribute::has_ongoing_buffs() const
{
	for (int i = 0; i < buffs.size(); i++) {
		if (const Ref<RuntimeBuff> buff = buffs[i]; !buff->can_dequeue() && !Math::is_zero_approx(buff->get_duration())) {
			return true;
		}
	}

	return false;
}

bool RuntimeAttribute::is_computable() const
{
	return GDVIRTUAL_IS_OVERRIDDEN_PTR(attribute, _compute_value);
}

bool RuntimeAttribute::remove_buff(const Ref<AttributeBuff> &p_buff)
{
	for (int i = 0; i < buffs.size(); i++) {
		if (const Ref<RuntimeBuff> buff = buffs[i]; buff->equals_to(p_buff)) {
			buffs.remove_at(i);
			emit_signal("buff_removed", buff);
			return true;
		}
	}

	return false;
}

Ref<Attribute> RuntimeAttribute::get_attribute() const
{
	return attribute;
}

Ref<AttributeSet> RuntimeAttribute::get_attribute_set() const
{
	return attribute_set;
}

float RuntimeAttribute::get_buff() const
{
	return buff_value;
}

float RuntimeAttribute::get_buffed_value() const
{
	float add_sub_total = value;
	float div_mul_multiplier = 1.0f;

	for (int i = 0; i < buffs.size(); i++) {
		const Ref<RuntimeBuff> buff = buffs[i];

		if (Ref<AttributeBuff> attribute_buff = buff->get_buff(); attribute_buff.is_valid()) {
			switch (attribute_buff->get_operation()->get_operand()) {
				case OP_ADD:
				case OP_SUBTRACT:
					add_sub_total = attribute_buff->operate(add_sub_total);
					break;
				case OP_MULTIPLY:
				case OP_DIVIDE:
				case OP_PERCENTAGE:
					div_mul_multiplier = attribute_buff->operate(div_mul_multiplier);
					break;
				default:
					break;
			}
		}
	}

	return add_sub_total * div_mul_multiplier;
}

TypedArray<AttributeBase> RuntimeAttribute::get_derived_from() const
{
	if (GDVIRTUAL_IS_OVERRIDDEN_PTR(attribute, _derived_from)) {
		if (TypedArray<AttributeBase> derived_attributes; GDVIRTUAL_CALL_PTR(attribute, _derived_from, attribute_set, derived_attributes)) {
			return derived_attributes;
		}
	}

	return {};
}

float RuntimeAttribute::get_previous_value() const
{
	return previous_value;
}

float RuntimeAttribute::get_value() const
{
	return value;
}

TypedArray<RuntimeBuff> RuntimeAttribute::get_buffs() const
{
	return buffs;
}

void RuntimeAttribute::set_attribute(const Ref<AttributeBase> &p_value)
{
	attribute = p_value;
}

void RuntimeAttribute::set_attribute_set(const Ref<AttributeSet> &p_value)
{
	attribute_set = p_value;
}

void RuntimeAttribute::set_buff(const float p_value)
{
	buff_value = p_value;
}

void RuntimeAttribute::set_value(const float p_value)
{
	previous_value = value;
	value = p_value;
}

void RuntimeAttribute::_bind_methods()
{
	/// binds methods to godot
	ClassDB::bind_method(D_METHOD("add_buff", "p_buff"), &RuntimeAttribute::add_buff);
	ClassDB::bind_method(D_METHOD("can_receive_buff", "p_buff"), &RuntimeAttribute::can_receive_buff);
	ClassDB::bind_method(D_METHOD("clear_buffs"), &RuntimeAttribute::clear_buffs);
	ClassDB::bind_method(D_METHOD("get_attribute"), &RuntimeAttribute::get_attribute);
	ClassDB::bind_method(D_METHOD("get_attribute_name"), &RuntimeAttribute::get_attribute_name);
	ClassDB::bind_method(D_METHOD("get_attribute_set"), &RuntimeAttribute::get_attribute_set);
	ClassDB::bind_method(D_METHOD("get_buffed_value"), &RuntimeAttribute::get_buffed_value);
	ClassDB::bind_method(D_METHOD("get_buff"), &RuntimeAttribute::get_buff);
	ClassDB::bind_method(D_METHOD("get_buffs"), &RuntimeAttribute::get_buffs);
	ClassDB::bind_method(D_METHOD("get_derived_from"), &RuntimeAttribute::get_derived_from);
	ClassDB::bind_method(D_METHOD("get_parent_runtime_attributes"), &RuntimeAttribute::get_parent_runtime_attributes);
	ClassDB::bind_method(D_METHOD("get_value"), &RuntimeAttribute::get_value);
	ClassDB::bind_method(D_METHOD("has_ongoing_buffs"), &RuntimeAttribute::has_ongoing_buffs);
	ClassDB::bind_method(D_METHOD("remove_buff", "p_buff"), &RuntimeAttribute::remove_buff);
	ClassDB::bind_method(D_METHOD("set_attribute", "p_value"), &RuntimeAttribute::set_attribute);
	ClassDB::bind_method(D_METHOD("set_attribute_set", "p_value"), &RuntimeAttribute::set_attribute_set);
	ClassDB::bind_method(D_METHOD("set_buff", "buff_value"), &RuntimeAttribute::set_buff);
	ClassDB::bind_method(D_METHOD("set_value", "p_value"), &RuntimeAttribute::set_value);

	/// binds properties to godot
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "attribute", PROPERTY_HINT_RESOURCE_TYPE, "AttributeBase"), "set_attribute", "get_attribute");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "buff"), "set_buff", "get_buff");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "value"), "set_value", "get_value");

	/// adds signals to godot
	ADD_SIGNAL(MethodInfo("attribute_changed", PropertyInfo(Variant::OBJECT, "attribute", PROPERTY_HINT_RESOURCE_TYPE, "RuntimeAttributeBase"), PropertyInfo(Variant::FLOAT, "previous_value"), PropertyInfo(Variant::FLOAT, "new_value")));
	ADD_SIGNAL(MethodInfo("buff_added", PropertyInfo(Variant::OBJECT, "buff", PROPERTY_HINT_RESOURCE_TYPE, "RuntimeBuff")));
	ADD_SIGNAL(MethodInfo("buff_removed", PropertyInfo(Variant::OBJECT, "buff", PROPERTY_HINT_RESOURCE_TYPE, "RuntimeBuff")));
	ADD_SIGNAL(MethodInfo("buff_time_updated", PropertyInfo(Variant::OBJECT, "buff", PROPERTY_HINT_RESOURCE_TYPE, "RuntimeBuff")));
	ADD_SIGNAL(MethodInfo("buffs_cleared"));
}

#pragma endregion