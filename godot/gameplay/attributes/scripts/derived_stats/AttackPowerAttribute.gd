class_name AttackPowerAttribute
extends Attribute

const ATTRIBUTE_NAME := "AttackPowerAttribute"

func _init(_attribute_name = ATTRIBUTE_NAME) -> void:
	attribute_name = _attribute_name
	
	
func _compute_value(attribute_computation_argument: AttributeComputationArgument) -> float:
	var parent_attributes := attribute_computation_argument.runtime_attribute.get_parent_runtime_attributes()
	var main_attribute: float
	
	match int(parent_attributes[0].get_buffed_value()):
		0: main_attribute = parent_attributes[1].get_buffed_value()
		1: main_attribute = parent_attributes[2].get_buffed_value()
		2: main_attribute = parent_attributes[3].get_buffed_value()

	var base = 1.0  # it would be hilarious to allow negative values. I cast a fireball to you, you gain health. 
	var weapon_damage 	= parent_attributes[4].get_buffed_value()
	var weapon_speed	= parent_attributes[5].get_buffed_value()
	var critical_change	= parent_attributes[6].get_buffed_value()
	var attack_power = (base + (weapon_damage * weapon_speed)) + floorf(main_attribute / 10.0)

	return attack_power + ((attack_power / 100) * critical_change)


func _derived_from(attribute_set: AttributeSet) -> Array[AttributeBase]:
	return [
		attribute_set.find_by_name(MainStatAttribute.ATTRIBUTE_NAME),
		attribute_set.find_by_name(DexterityAttribute.ATTRIBUTE_NAME),
		attribute_set.find_by_name(IntelligenceAttribute.ATTRIBUTE_NAME),
		attribute_set.find_by_name(StrengthAttribute.ATTRIBUTE_NAME),
		attribute_set.find_by_name(WeaponDamageAttribute.ATTRIBUTE_NAME),
		attribute_set.find_by_name(WeaponSpeedAttribute.ATTRIBUTE_NAME),
		attribute_set.find_by_name(CriticalChanceAttribute.ATTRIBUTE_NAME),
	]
