AttributeBuff
=================

defined in `src/attribute.hpp`

## Description

An `AttributeBuff` is a class which describes how it modifies an attribute.

It can be used directly as a resource to modify attributes directly or
can be extended to create more complex buffs.

> **Note:** Remember to instance those as a `.tres` resource if you need
> to squeeze the performance of your game.

## Methods

## _apply

This (optional) virtual method gives you **full** control over the buff calculation and life-time.

It receives a single parameter of type [`AttributeBuffContext`](AttributeBuffContext.md) which contains all the
information and methods to handle a buff.

example:

```gdscript
class_name HealMeOnce
extends AttributeBuff


# a simple buff that adds 10 health to the character.
func _apply(context: AttributeBuffContext) -> void:
    # we instantiate a new change set
    var changeset = context.new_changeset()

    # we retrieve the health attribute from the context and we add 10 to it
    changeset.operate(context.get_attribute("health"), AttributeOperation.add(10.0))

    # we commit the changes
    context.commit(changeset)
```

This opens the possibility to create complex buffs with multiple operations and multiple attributes in a single place.

Another peculiarity is the possibility to handle the buff life-time, with multiple applications for each "tick" (or for
a fixed span of time).

```gdscript
class_name HealMeOverTime 
extends AttributeBuff


# let's imagine a "restore health" buff that restores 10 health every second for 10 seconds
func _apply(context: AttributeBuffContext) -> void:
    var changeset = context.new_changeset()
    
    # we add 10 to the health attribute but we want to manage the operation further
    var operation = changeset.operate(context.get_attribute("health"), AttributeOperation.add(10.0))

    # this tells the container to give a duration to the operation with a unit of measure in seconds.
    # in this specific case, the buff will last for 10 seconds.
    operation.set_duration(10.0, AttributeBuffContext.TICK_SECOND)
    
    # we tell the container to reapply the operation every tick.
    # this means that the buff will be applied every second for 10 seconds.
    operation.reapply_every_tick()

    # we commit the changes
    context.commit(changeset)
```

Another example of time-based buffs is the poison damage buff.

```gdscript
class_name PoisonDamage 
extends AttributeBuff


# let's imagine a stacking damage buff that applies 1 damage every second for 10 seconds.
func _apply(context: AttributeBuffContext) -> void:
    # I forgot to mention that we can add an optional name to the changeset.
    # This can be useful to identify the changeset in the container and to rollback it if needed.
    var changeset = context.new_changeset("poison_damage") 

    if context.has_changeset("poison_damage"):
        # We rollback the previous operation if it exists. 
        # This is useful to reset a buff entirely you need to renew it's duration for example.
        context.rollback("poison_damage") 

    # we subtract 1.0 
    var operation = changeset.operate(context.get_attribute("health"), AttributeOperation.sub(1.0))

    # we tell the container to give a duration to the operation with a unit of measure in seconds.
    # in this specific case, the buff will last for 10 seconds.
    operation.set_duration(10.0, AttributeBuffContext.TICK_SECOND)

    # again, we tell the container to reapply the operation every tick.
    operation.reapply_every_tick() # we apply the damage every tick.
    
    # let's commit changes
    context.commit(changeset)
```

Proceeding, we were poisoned, but we found a cure!

```gdscript
class_name CureMeFromPoisonPls 
extends AttributeBuff


# the cure removes the poison stacks from the character.
func _apply(context: AttributeBuffContext) -> void:
    # we clear the stacks of the poison damage.
    context.rollback("poison_damage")
```

The boss dropped new armor with a few interesting new stats, let's equip it!

```gdscript
class_name CowKingsHideArmor 
extends AttributeBuff


func _apply(context: AttributeBuffContext) -> void:
    # again, we want to ensure that we have a unique changeset name that can be rolled back.
    var changeset = context.new_changeset("cow_kings_hide_armor")

    # we add 60 armor to the character.
    changeset.operate(context.get_attribute("armor"), AttributeOperation.add(60.0))
    
    # this armor also applies a +10% global armor bonus.
    changeset.operate(context.get_attribute("armor"), AttributeOperation.perc(10.0))
    
    # it increases max health by 60 (if I do remember D2 well, it was about 20 years ago)
    var max_health_operation = changeset.operate(context.get_attribute("max_health"), AttributeOperation.add(10.0))

    # we increase the health current value by the difference of actual health/max_health and the new max_health
    var current_health = context.get_attribute("health").get_current_value()
    var max_health = context.get_attribute("max_health").get_current_value()
    var resulting_max_health = max_health_operation.get_resulting_value()
    var new_health = (resulting_max_health - max_health) - (max_health - current_health);
    var health_operation = changeset.operate(context.get_attribute("health"), AttributeOperation.add(new_health)

    # yes, again, commit, yuhu    
    context.commit(changeset)
```

#### _applies_to (deprecated, use _apply instead)

> important: this will be removed as soon as we reach a stable 1.0

This method is used to gather the attributes that this buff applies to.

This function accepts one argument of type [`AttributeSet`](AttributeSet.md) and returns an array of [
`Attribute`](Attribute.md).

You should override this function if:

- You want to apply the buff to a specific attribute.
- You want to gather more attributes to use them in the buff calculation.

```gdscript
func _applies_to(attribute_set: AttributeSet) -> Array[AttributeBase]
```

#### _operate (deprecated, use _apply instead)

> important: this will be removed as soon as we reach a stable 1.0

This method is used to calculate all the operations applied to every attribute
in the array returned by `_applies_to`.

This function accepts these arguments:

- `values` of type `Array[float]` - The values of the attributes (as defined by `_applies_to`) to apply the buff to.
- `attribute_set` of type [`AttributeSet`](AttributeSet.md) - The attribute set to use to apply the buff.

This function must return an array of [`AttributeOperation`](AttributeOperation.md) which will be used to modify the
attributes.

You should override this function if:

- You want to apply a specific operation to the attributes.
- You want to apply multiple operations to many attributes.
- You want to initialize the attributes in an `AttributeSet` with a specific value.

```gdscript
func _operate(values: Array[float], attribute_set: AttributeSet) -> Array[AttributeOperation]
```

[Back to classes](README.md)
