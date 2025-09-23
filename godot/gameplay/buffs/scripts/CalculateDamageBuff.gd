class_name CalculateDamageBuff
extends AttributeBuff


func _applies_to(attribute_set: AttributeSet) -> Array[AttributeBase]:
	return [
		attribute_set.find_by_name(HealthAttribute.ATTRIBUTE_NAME),
		attribute_set.find_by_name(ArmorAttribute.ATTRIBUTE_NAME),
		attribute_set.find_by_name(ResistanceDarkness.ATTRIBUTE_NAME),
		attribute_set.find_by_name(ResistanceFire.ATTRIBUTE_NAME),
		attribute_set.find_by_name(ResistanceLightning.ATTRIBUTE_NAME),
		attribute_set.find_by_name(ResistancePoison.ATTRIBUTE_NAME),
		attribute_set.find_by_name(DamageDarknessAttribute.ATTRIBUTE_NAME),
		attribute_set.find_by_name(DamageFireAttribute.ATTRIBUTE_NAME),
		attribute_set.find_by_name(DamageLightningAttribute.ATTRIBUTE_NAME),
		attribute_set.find_by_name(DamagePhysicalAttribute.ATTRIBUTE_NAME),
		attribute_set.find_by_name(DamagePoisonAttribute.ATTRIBUTE_NAME),
	]


func _operate(values: Array[float], _attribute_set: AttributeSet) -> Array[AttributeOperation]:
	var armorAttribute = values[1]
	var resistanceDarkness = values[2]
	var resistanceFire = values[3]
	var resistanceLightning = values[4]
	var resistancePoison = values[5]
	var damageDarknessAttribute = values[6]
	var damageFireAttribute = values[7]
	var damageLightningAttribute = values[8]
	var damagePhysicalAttribute = values[9]
	var damagePoisonAttribute = values[10]

	damageDarknessAttribute -= damageDarknessAttribute * (resistanceDarkness / 100)
	damageFireAttribute -= damageFireAttribute * (resistanceFire / 100)
	damageLightningAttribute -= damageLightningAttribute * (resistanceLightning / 100)
	damagePhysicalAttribute -= damagePhysicalAttribute * (armorAttribute / 1000)
	damagePoisonAttribute -= damagePoisonAttribute * (resistancePoison / 100)

	return [
		# we subtract the sum of all damage sources, removing it from the health attribute
		AttributeOperation.subtract(damageDarknessAttribute + damageFireAttribute + damageLightningAttribute + damagePhysicalAttribute + damagePoisonAttribute),
		# we do not want to update the values in any way
		AttributeOperation.add(0),
		AttributeOperation.add(0),
		AttributeOperation.add(0),
		AttributeOperation.add(0),
		AttributeOperation.add(0),
		# now let's reset damage values
		AttributeOperation.forcefully_set_value(0),
		AttributeOperation.forcefully_set_value(0),
		AttributeOperation.forcefully_set_value(0),
		AttributeOperation.forcefully_set_value(0),
		AttributeOperation.forcefully_set_value(0),
	]
