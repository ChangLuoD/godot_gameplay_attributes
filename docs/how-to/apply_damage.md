How to apply damage using attributes
========================

Damage is the most common operation in games.

You could receive it by getting hit by a bullet, by falling from a cliff, 
or by being attacked by a monster and so on.

In this section, we will see two simple examples 
on how to apply damage using attribute buffs.

> prerequisite: follow the [First Steps](first-steps.md) tutorial to create the `HealthAttribute` and `MaxHealthAttribute` attributes.

## Example 1, the simple way

In this example we are going to create a simple damage buff that is going
to be applied to the `HealthAttribute` attribute.

The logic behind this is straightforward, we are going 
to create a `DamageBuff` class which exports a value (float) which is going 
to be subtracted from the `HealthAttribute` attribute.

```gdscript
class_name DamageBuff
extends AttributeBuff

@export var damage: float = 1.0


func _init(damage: float):
  self.damage = damage
  attribute_name = HealthAttribute.ATTRIBUTE_NAME
  operation = AttributeOperation.subtract(damage)
```

Apply the buff to the `HealthAttribute` attribute like this:

```gdscript
attribute_container.apply_buff(DamageBuff.new(10.0))
```

That's all folks!
