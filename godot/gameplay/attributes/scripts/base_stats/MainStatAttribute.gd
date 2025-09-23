class_name MainStatAttribute 
extends Attribute

const ATTRIBUTE_NAME := "MainStatAttribute"


func _init(_attribute_name = ATTRIBUTE_NAME) -> void:
	attribute_name = _attribute_name


func _compute_value(attribute_computation_argument: AttributeComputationArgument) -> float:
	assert([0.0, 1.0, 2.0].has(attribute_computation_argument.operated_value), "invalid value, accepted are Dexterity (0), Intelligence (1) or Strength (2)" )	
	return attribute_computation_argument.operated_value
