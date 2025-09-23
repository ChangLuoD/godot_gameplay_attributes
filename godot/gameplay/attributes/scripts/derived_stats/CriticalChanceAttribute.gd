class_name CriticalChanceAttribute 
extends Attribute

const ATTRIBUTE_NAME := "CriticalChanceAttribute"

func _init(_attribute_name = ATTRIBUTE_NAME) -> void:
	attribute_name = _attribute_name


func _compute_value(attribute_computation_argument: AttributeComputationArgument) -> float:
	var parent_attributes 	= attribute_computation_argument.runtime_attribute.get_parent_runtime_attributes()
	var base_chance 		= parent_attributes[0]
	var dexterity			= parent_attributes[1]
	
	### base chance + 2% of dexterity
	return base_chance.get_buffed_value() + (dexterity.get_buffed_value() * 0.01)
	
	
func _derived_from(attribute_set: AttributeSet) -> Array[AttributeBase]:
	return [
		attribute_set.find_by_name(BaseCriticalChanceAttribute.ATTRIBUTE_NAME),
		attribute_set.find_by_name(DexterityAttribute.ATTRIBUTE_NAME)
	]
