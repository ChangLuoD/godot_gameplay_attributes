First steps
==================

This is a simple guide to get you started with the Godot Gameplay Attributes plugin.

This example is really simple but should cover the basics.

## Creating a new attribute

First thing first, let's create two attributes: `HealthAttribute` and `MaxHealthAttribute`.

When creating a new attribute, it's recommended to 
- specify the `class_name` in the script.
- define a constant for the attribute name (use the same constant name in all your attributes, it's always a good thing following the same convention in a codebase).
- write the `_init` method to set the attribute name as in the example below.

_res://attributes/MaxHealthAttribute.gd_ the script for the `MaxHealthAttribute` class:

```gdscript
class_name MaxHealthAttribute
extends Attribute

const ATTRIBUTE_NAME = "MaxHealth"

func _init(_attribute_name := ATTRIBUTE_NAME):
    attribute_name = _attribute_name
```

_res://attributes/HealthAttribute.gd_ the script for the `HealthAttribute` class:

```gdscript
class_name HealthAttribute
extends Attribute

const ATTRIBUTE_NAME = "Health"


func _init(_attribute_name := ATTRIBUTE_NAME):
    attribute_name = _attribute_name
    

## We are telling the AttributeContainer that this attribute (HealthAttribute)
## is derived from the MaxHealthAttribute.
func _derived_from(attribute_set: AttributeSet) -> Array[AttributeBase]:
    return [
        attribute_set.find_by_name(MaxHealthAttribute.ATTRIBUTE_NAME),
    ]
    

## We want to avoid health overflowing the max health.
## We are going to use the _compute_value method to do that.
func _compute_value(argument: AttributeComputationArgument) -> float:
    var parent_attributes := argument.get_parent_attributes()
    var max_health_attribute := parent_attributes[0] ## since the array is ordered as we defined in _derived_from
    
    # Clamp the value between 0 (min health, fixed value) and MaxHealthAttribute's value
    return clamp(argument.operated_value, 0, max_health_attribute.get_buffed_value()) ## we asked for the buffed value, so if the max health increases, the maximum value of health will be able to increase too.
```

## Instancing the attributes as resources

It's time
to create an instance of these attributes.

To do that,
right-click on the FileSystem panel
to create a new resource
(one for each attribute).

After that, we need to create a new `AttributeSet` to hold our attributes. 

To do that, open the file explorer in godot, click with the right button of the mouse in the folder where you want to create the new file and select `New Resource...`. 
Then select `AttributeSet` and click `Create`.

Once saved the resource,
double-click on it and:
- At the top of the inspector, click the |+| button twice to increase the number of values.
- Then drag and drop the `MaxHealthAttribute` and `HealthAttribute` resource.

Now save the resource and back to the scene.

## Assigning the attributes to an AttributeContainer node

Add a node to your scene, select the `AttributeContainer`

Double-click on it and at the top of the inspector,
drag and drop the new `AttributeSet` resource
you just created.

Now, how do we set a starting value? 

To set an initial value to the attributes, we need to run an AttributeBuff on them when the
AttributeContainer come in game.

## Initializing attributes values

To do that, we need to create our first [`AttributeBuff`](../classes/AttributeBuff.md).

Let's create a new script called `InitializeAttributesBuff.gd` and add the following code:

```gdscript
class_name InitializeAttributesBuff
extends AttributeBuff

@export var max_health: float = 100.0
@export var health: float = 100.0

func _applies_to(attribute_set: AttributeSet) -> Array[AttributeBase]:
    # note: remember to apply a buff to "base" attributes first and to "derived" later
    return [
        attribute_set.find_by_name(MaxHealthAttribute.ATTRIBUTE_NAME),
        attribute_set.find_by_name(HealthAttribute.ATTRIBUTE_NAME),
    ]


func _operate(_values, _attribute_set: AttributeSet) -> Array[AttributeOperation]:
    # note: the order must be the same as  the one in _applies_to
    return [
        AttributeOperation.forcefully_set_value(max_health),
        AttributeOperation.forcefully_set_value(health),
    ]
```

Now, into our scene's scrit, we have to instance and apply this buff

```gdscript
func _ready() -> void:
    attribute_container.apply_buff(InitializeAttributesBuff.new())
```

Done! 
