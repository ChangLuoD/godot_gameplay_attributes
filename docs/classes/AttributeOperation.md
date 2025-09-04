AttributeOperation
=================

defined in `src/attribute.hpp`

## Description

Represents an operation that can be applied to an attribute by an [`AttributeBuff`](AttributeBuff.md).

The operation can be one of the following:

- `AttributeOperation.ADD`: adds the value to the attribute.
- `AttributeOperation.SUBTRACT`: subtracts the value from the attribute.
- `AttributeOperation.MULTIPLY`: multiplies the value with the attribute.
- `AttributeOperation.DIVIDE`: divides the attribute by the value.
- `AttributeOperation.SET`: sets the attribute to the value.

> **Note:** `AttributeOperation.SET` will not work inside the `AttributeBuff._apply` method.

## Methods

- **static** `add`: creates an `AttributeOperation` that adds a value to the attribute.
- **static** `divide`: creates an `AttributeOperation` that divides the attribute by a value.
- **static** `multiply`: creates an `AttributeOperation` that multiplies the attribute by a value.
- **static** `percentage`: creates an `AttributeOperation` that multiplies the attribute by a percentage.
- **static** `subtract`: creates an `AttributeOperation` that subtracts a value from the attribute.
- **static** `forcefully_set_value`: creates an `AttributeOperation` that sets the attribute to a value.
- `operate`: calculates the result of applying the operation to a given value and returns the result without affecting the value itself.

[Back to classes](README.md)
