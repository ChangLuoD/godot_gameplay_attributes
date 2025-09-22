## ATTENTION!
## thank you for your attention
class_name ExperienceBuff
extends AttributeBuff


@export var minimum_experience: float = 1.0
@export var maximum_experience: float = 5.0


func _applies_to(attribute_set: AttributeSet) -> Array[AttributeBase]:
	## note: here I am currently stressing the system a bit.
	## this should not have much side-effects on performance,
	## but it would be a better approach to update dex/int/str attributes
	## while listening to the LevelAttribute.changed signal
	return [
		attribute_set.find_by_name(LevelAttribute.ATTRIBUTE_NAME),
		attribute_set.find_by_name(ExperienceAttribute.ATTRIBUTE_NAME),
		attribute_set.find_by_name(NextLevelExperience.ATTRIBUTE_NAME),
		attribute_set.find_by_name(MainStatAttribute.ATTRIBUTE_NAME),
		attribute_set.find_by_name(DexterityAttribute.ATTRIBUTE_NAME),
		attribute_set.find_by_name(IntelligenceAttribute.ATTRIBUTE_NAME),
		attribute_set.find_by_name(StrengthAttribute.ATTRIBUTE_NAME),
		attribute_set.find_by_name(LevelCap.ATTRIBUTE_NAME),
		attribute_set.find_by_name(LevelAttribute.ATTRIBUTE_NAME),
	]


func _operate(values: Array[float], _attribute_set: AttributeSet)	-> Array[AttributeOperation]:
	var current_level 			= values[0]
	var current_experience 		= values[1]
	var next_level_experience	= values[2]
	var main_stat				= int(values[3])
	var dex_stat				= values[4]
	var int_stat				= values[5]
	var str_stat				= values[6]
	var level_cap				= values[7]
	var level					= values[8]
	var new_exp					= round(current_level + (current_level * randf_range(minimum_experience, maximum_experience)))
	var experience_operation 	= AttributeOperation.add(new_exp)
	var boosted_experience		= experience_operation.operate(current_experience)
	var ret: Array[AttributeOperation] = []
	var level_up := false

	if boosted_experience >= next_level_experience && level_cap >= level + 1:
		level_up = true
		ret = [
			AttributeOperation.add(1),
			AttributeOperation.forcefully_set_value(maxf(abs(boosted_experience - next_level_experience), 0.0)),
			AttributeOperation.add(0),
		]
	elif level_cap == level:
		ret = [
			AttributeOperation.add(0),
			AttributeOperation.add(0),
			AttributeOperation.add(0),
		]
	else:
		ret = [
			AttributeOperation.add(0),
			experience_operation,
			AttributeOperation.add(0), # we do not care to alter this but we need to return it to make things work
		]
	
	ret.append_array(get_stat_op(level_up, main_stat, dex_stat, int_stat, str_stat))
	
	return ret
	

func get_stat_op(level_up: bool, main_stat: int, dex_stat: float, int_stat: float, str_stat: float) -> Array[AttributeOperation]:
	if level_up:
		var dex_stat_op := 1.0
		var int_stat_op := 1.0
		var str_stat_op := 1.0
		
		match main_stat:
			0: dex_stat_op = 5.0
			1: int_stat_op = 5.0
			2: str_stat_op = 5.0
		
		return [
			AttributeOperation.forcefully_set_value(main_stat),
			AttributeOperation.add(dex_stat_op),
			AttributeOperation.add(int_stat_op),
			AttributeOperation.add(str_stat_op),
		]
	
	return [
		AttributeOperation.forcefully_set_value(main_stat),
		AttributeOperation.forcefully_set_value(dex_stat),
		AttributeOperation.forcefully_set_value(int_stat),
		AttributeOperation.forcefully_set_value(str_stat),
	]
