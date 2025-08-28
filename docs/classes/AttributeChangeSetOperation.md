AttributeChangeSetOperation
===========================

## Methods

#### get_resulting_value

Returns the resulting value of the operation.

#### reset_duration

Resets the ongoing duration of a similar operation applied on the same attribute in a previous change set.

#### set_duration

Sets the duration of the operation.

This function accepts two parameters:

- `duration`: The duration of the operation.
- `unit_of_measure` (optional, defaults to milliseconds): the unit of measure of the duration (milliseconds, seconds, minutes).

#### reapply_every_tick

If this function is called with `true`, the operation will be reapplied every tick.
