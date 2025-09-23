extends VBoxContainer

@onready var current_level_bar: ProgressBar = %CurrentLevelBar
@onready var attribute_container: AttributeContainer = %AttributeContainer
@onready var gain_xp_button: Button = %GainXpButton
@onready var weapon_selection_option_button: OptionButton = %WeaponSelectionOptionButton
@onready var main_stat_option_button: OptionButton = %MainStatOptionButton
@onready var gain_xp_over_time_check_box: CheckBox = %GainXPOverTimeCheckBox
@onready var heal_me_button: Button = %HealMeButton


func _ready() -> void:
	attribute_container.apply_buff(InitializeAttributes.new())
	attribute_container.attribute_changed.connect(update_ui)

	gain_xp_button.pressed.connect(func () -> void:
		attribute_container.apply_buff(ExperienceBuff.new())
	)
	
	heal_me_button.pressed.connect(func () -> void:
		attribute_container.apply_buff(PotionBuff.new())	
	)
	
	main_stat_option_button.item_selected.connect(func (item_index: int) -> void:
		attribute_container.apply_buff(SetInitialValueBuff.create(MainStatAttribute.ATTRIBUTE_NAME, item_index))
	)
	
	weapon_selection_option_button.item_selected.connect(func (item_index: int) -> void:
		attribute_container.apply_buff(WeaponDamageBuff.new(item_index))
	)
	
	var timer := Timer.new()
	timer.wait_time = .1
	timer.timeout.connect(func ():
		if gain_xp_over_time_check_box.button_pressed:
			attribute_container.apply_buff(ExperienceBuff.new())	
	)
	add_child(timer)
	timer.start()


func update_ui(runtime_attribute: RuntimeAttribute, _old_value: float, new_value: float) -> void:
	if runtime_attribute.attribute is ToNextLevelPercentageAttribute:
		current_level_bar.value = new_value
	if runtime_attribute.attribute is DamageAttribute:
		attribute_container.apply_buff(CalculateDamageBuff.new())
