class_name EvasionAttribute 
extends Attribute

const ATTRIBUTE_NAME := "EvasionAttribute"

func _init(_attribute_name = ATTRIBUTE_NAME) -> void:
	attribute_name = _attribute_name


func _compute_value(attribute_computation_argument: AttributeComputationArgument) -> float:
	# Evasion is a base of 2% + 0.1% for each 10 points of dexterity.
	# Max value is 45%. It's ok to be hard to hit but not that much, c'mon.
	var base 		:= 2.0
	var factor	 	:= int(attribute_computation_argument.runtime_attribute.get_parent_runtime_attributes()[0].get_buffed_value()) % 10
	var output		:= base * (0.1 * factor)
	
	return clampf(output, 0.0, 45.0)


func _derived_from(attribute_set: AttributeSet) -> Array[AttributeBase]:
	return [
		attribute_set.find_by_name(DexterityAttribute.ATTRIBUTE_NAME)
	]
