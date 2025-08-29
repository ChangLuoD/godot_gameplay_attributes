How to make a RPG levelling system
========================

## Description

It's very common in RPG games to have a levelling system.

Usually a character has a level, a value for experience and other "accessories" like
- next level experience
- next level

Once a character levels up, some stats are increased, like health, mana, strength, etc.

You can handle those stats with the `Attribute` system in a very easy way.

## How should it work

1. The character has a level.
2. The character has a value for experience.
3. The character has a value for next level experience, which is derived both from level and experience and calculated in a `x` way.
4. When the character gains experience
   - the experience value is increased
   - if the experience value is greater than or equal to the next level experience, the character levels up.
     - the remaining experience is calculated as `experience - next_level_experience` and reassigned to the experience attribute.
     - the level is increased by 1.
   - if there is a hard cap on level, the level is clamped to the maximum value and checked before getting experience.

## Steps

1. Create a new `Attribute` class for the level.
   - This attribute will be used to store the level of the character.
   - If you need a "hard cap" on level, you can choose to create a `MaxLevelAttribute` class or you could simply export a value from this attribute.
2. Create a new `Attribute` class for the experience.
3. Create a new `Attribute` class for the next level experience.

Experience and Level attributes are considered to be "base" attributes, while the 'next level experience' is a "derived" attribute.

To handle the levelling, you will perform all calculations in an `AttributeBuff` class.

We need to do two things: set up the attributes and write the buff logic.

#### Setup attributes

Create a Level attribute.

This attribute will be used to store the level of the character.

```gdscript
class_name LevelAttribute
extends Attribute

const ATTRIBUTE_NAME = "Level"

@export var max_level: int = 60 ## hard cap on level, like in diablo games

func _init(_attribute_name := ATTRIBUTE_NAME):
    attribute_name = _attribute_name
    
    
func _compute_value(argument: AttributeComputationArgument) -> float:
    ## 1 is the minimum level, we clamp the level value to 1 and the exported max_level
    return clampf(argument.operated_value, 1, max_level)
```

Create an Experience attribute:

```gdscript
class_name ExperienceAttribute
extends Attribute

const ATTRIBUTE_NAME = "Experience"

func _init(_attribute_name := ATTRIBUTE_NAME):
    attribute_name = _attribute_name
```

Create a NextLevelExperience attribute (this is a derived attribute):

```gdscript
class_name NextLevelExperienceAttribute
extends Attribute

const ATTRIBUTE_NAME = "NextLevelExperience"
const EXPERIENCE_PER_LEVEL 	:= 20.0 # Base experience per level
const SCALE 				:= 1.5  # Scale for the experience curve


func _init(_attribute_name := ATTRIBUTE_NAME):
    attribute_name = _attribute_name

func _derived_from(attribute_set: AttributeSet) -> Array[Attribute]:
    return [
        attribute_set.find_by_name(LevelAttribute.ATTRIBUTE_NAME),
    ]
    
func _compute_value(argument: AttributeComputationArgument) -> float:
    var level_attribute := argument.get_parent_attributes()[0]
    var level := level_attribute.get_value()
    
    # The next level experience is calculated as 100 * level
    return round(((current_level * SCALE) ** 1.5) * EXPERIENCE_PER_LEVEL)
```

Create the `GetExperienceBuff` class, which also levels up the character if needed.

```gdscript
class_name GetExperienceBuff
extends AttributeBuff


@export var min_experience_gained: float = 3.0      # Minimum experience gained from the buff
@export var max_experience_gained: float = 10.0     # Maximum experience gained from the buff


func _init(_min_experience_gained: float, _max_experience_gained: float):
    min_experience_gained = _min_experience_gained
    max_experience_gained = _max_experience_gained


func _apply(context: AttributeBuffContext) -> void:
    var changeset = context.new_changeset()
    
    ## we calculate the experience gained based on the exported values
    var exp_gained              := randf_range(min_experience_gained, max_experience_gained)
    
    ## we retrieves a few attributes, we have to read some values first
    var level_attribute         := context.get_attribute(LevelAttribute.ATTRIBUTE_NAME)
    var experience              := context.get_attribute(ExperienceAttribute.ATTRIBUTE_NAME)
    var next_level_experience   := context.get_attribute(NextLevelExperienceAttribute.ATTRIBUTE_NAME)
    
    ## if the level is equal to the max level exported by LevelAttribute, we do nothing
    if level_attribute.get_value() == level_attribute.max_level:
        return
    
    ## we compute the new experiece value
    var new_experience          := exp_gained + experience.get_value()
    
    ## we have the new experience overflowing `next_level_experience`, it's a level up!
    if new_experience >= next_level_experience:
        changeset.operate(LevelAttribute.ATTRIBUTE_NAME, AttributeOperation.add(1))
        changeset.operate(ExperienceAttribute.ATTRIBUTE_NAME, AttributeOperation.add(new_experience - next_level_experience))
    else:
        changeset.operate(ExperienceAttribute.ATTRIBUTE_NAME, AttributeOperation.add(new_experience))

    context.commit(changeset)
```

Apply the buff and enjoy.