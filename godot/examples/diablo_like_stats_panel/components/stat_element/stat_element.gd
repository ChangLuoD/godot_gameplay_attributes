extends HBoxContainer

@export var attribute: Attribute					 = null
@export var attribute_container: AttributeContainer	 = null
@export var decimals							   	:= 2


@onready var stat_name: Label	= %StatName
@onready var stat_value: Label	= %StatValue


func _ready() -> void:
	assert(attribute != null, "attribute is null, do something c'mon")
	assert(attribute_container != null, "attribute container is null, did you do your homework?")
	attribute_container.attribute_changed.connect(on_attribute_changed)
	render(attribute)


func on_attribute_changed(p_attribute: RuntimeAttribute, _previous_value: float, _new_value: float) -> void:
	if attribute.attribute_name == p_attribute.attribute.attribute_name:
		render(p_attribute.attribute)


func render(p_attribute: Attribute) -> void:
	stat_name.text 	= p_attribute.attribute_name.replace("Attribute", "").to_snake_case().replace("_", " ").capitalize()
	stat_value.text = String.num(attribute_container.get_attribute_buffed_value_by_name(p_attribute.attribute_name), decimals)

	if decimals > 0:
		stat_value.text += "%"
