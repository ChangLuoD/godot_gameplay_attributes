AttributeBuffContext
====================

This is the only argument passed to the [AttributeBuff](./AttributeBuff.md) `_apply` function.

## Methods

#### commit

Commits an [AttributeChangeset](./AttributeChangeSet.md) instance to the container, updating the attribute values,
triggering signals, starting buff life-cycles, etc.

> in depth: the changes applied to the container are saved and stored in the [AttributeContainer](./AttributeContainer.md) node.
> the system understands if a changeset is transient or not by itself, and by itself I mean if you specify a name
> to a changeset or you set a time, the system will presume it is transient.

#### get_attribute

Gets a [`RuntimeAttribute`](./RuntimeAttribute.md) instance from the container.

Returns `null` if the attribute does not exist, please check with `has_attribute` first or check if the attribute is valid.

#### has_attribute

Checks if the container has an attribute with the given name.

#### has_changeset

Returns `true` if the context has a changeset with that given name, `false` otherwise.

#### new_changeset

Returns a new [AttributeChangeset](./AttributeChangeSet.md) object.

Use this to update your attribute values.

This function accepts an optional name (a `String`). If this argument is passed, the changeset will be transient.

#### rollback

Rolls back the changes made to the container by a named changeset.
