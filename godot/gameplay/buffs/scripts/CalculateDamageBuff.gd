class_name CalculateDamageBuff
extends AttributeBuff


func _apply(attribute_buff_context: AttributeBuffContext) -> void:
	var changeset = attribute_buff_context.new_changeset()
	var reset_incoming_damage_changeset = attribute_buff_context.new_changeset()
	var armorAttribute = attribute_buff_context.get_attribute(ArmorAttribute.ATTRIBUTE_NAME).get_buffed_value()
	var resistanceDarkness = attribute_buff_context.get_attribute(ResistanceDarkness.ATTRIBUTE_NAME).get_buffed_value()
	var resistanceFire = attribute_buff_context.get_attribute(ResistanceFire.ATTRIBUTE_NAME).get_buffed_value()
	var resistanceLightning = attribute_buff_context.get_attribute(ResistanceLightning.ATTRIBUTE_NAME).get_buffed_value()
	var resistancePoison = attribute_buff_context.get_attribute(ResistancePoison.ATTRIBUTE_NAME).get_buffed_value()
	var damageDarknessAttribute = attribute_buff_context.get_attribute(DamageDarknessAttribute.ATTRIBUTE_NAME).get_buffed_value()
	var damageFireAttribute = attribute_buff_context.get_attribute(DamageFireAttribute.ATTRIBUTE_NAME).get_buffed_value()
	var damageLightningAttribute = attribute_buff_context.get_attribute(DamageLightningAttribute.ATTRIBUTE_NAME).get_buffed_value()
	var damagePhysicalAttribute = attribute_buff_context.get_attribute(DamagePhysicalAttribute.ATTRIBUTE_NAME).get_buffed_value()
	var damagePoisonAttribute = attribute_buff_context.get_attribute(DamagePoisonAttribute.ATTRIBUTE_NAME).get_buffed_value()
	
	damageDarknessAttribute -= damageDarknessAttribute * (resistanceDarkness / 100)
	damageFireAttribute -= damageFireAttribute * (resistanceFire / 100)
	damageLightningAttribute -= damageLightningAttribute * (resistanceLightning / 100)
	damagePhysicalAttribute -= damagePhysicalAttribute * (armorAttribute / 1000)
	damagePoisonAttribute -= damagePoisonAttribute * (resistancePoison / 100)
	
	var all_damage = damageDarknessAttribute + damageFireAttribute + damageLightningAttribute + damagePhysicalAttribute + damagePoisonAttribute
	
	changeset.operate(HealthAttribute.ATTRIBUTE_NAME, AttributeOperation.subtract(all_damage))

	reset_incoming_damage_changeset.operate(DamageDarknessAttribute.ATTRIBUTE_NAME, AttributeOperation.forcefully_set_value(0))
	reset_incoming_damage_changeset.operate(DamageFireAttribute.ATTRIBUTE_NAME, AttributeOperation.forcefully_set_value(0))
	reset_incoming_damage_changeset.operate(DamageLightningAttribute.ATTRIBUTE_NAME, AttributeOperation.forcefully_set_value(0))
	reset_incoming_damage_changeset.operate(DamagePhysicalAttribute.ATTRIBUTE_NAME, AttributeOperation.forcefully_set_value(0))
	reset_incoming_damage_changeset.operate(DamagePoisonAttribute.ATTRIBUTE_NAME, AttributeOperation.forcefully_set_value(0))
	print(all_damage)
	attribute_buff_context.commit(changeset)
	attribute_buff_context.commit(reset_incoming_damage_changeset)
