AttributeChangeSet
==================

This class is used to store changes to attributes before they are applied.

Yes, this enables "transactional" changes to attributes which can be undone before they are committed.

> **Note:** This class is not meant to be used directly.
> 
> You can have only one AttributeChangeSetOperation per attribute at a time for each AttributeChangeSet you create.

## Methods

#### get_operations

Returns an array of [`AttributeChangeSetOperation`](./AttributeChangeSetOperation.md)s.

#### has_operations

Returns `true` if there are any operations in the set.

#### is_operating_attribute

Returns `true` if the given attribute is currently being operated on.

```gdscript
func is_operating_attribute(attribute: RuntimeAttribute) -> bool
```

#### operate

Enqueues an [`AttributeOperation`](./AttributeOperation.md) to be performed on a specific attribute.

Returns an instance of [`AttributeChangeSetOperation`](./AttributeChangeSetOperation.md)

```gdscript
func operate(attribute: RuntimeAttribute, operation: AttributeOperation) -> AttributeChangeSetOperation
```
