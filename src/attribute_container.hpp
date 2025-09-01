/**************************************************************************/
/*  attribute_container.hpp                                               */
/**************************************************************************/
/*                         This file is part of:                          */
/*                        Godot Gameplay Systems                          */
/*              https://github.com/OctoD/godot-gameplay-systems           */
/**************************************************************************/
/* Read the license file in this repo.						              */
/**************************************************************************/

#ifndef OCTOD_GAMEPLAY_ATTRIBUTES_ATTRIBUTE_CONTAINER_H
#define OCTOD_GAMEPLAY_ATTRIBUTES_ATTRIBUTE_CONTAINER_H

#include "attribute_buff.h"

#include <godot_cpp/classes/node.hpp>

using namespace godot;

namespace octod::gameplay::attributes
{
	class AttributeBase;
	class AttributeBuff;
	class AttributeSet;
	class RuntimeAttribute;
	class RuntimeBuff;

	// ReSharper disable once CppClassCanBeFinal
	class AttributeContainer : public Node
	{
		GDCLASS(AttributeContainer, Node);

	public:
		/// @brief Adds an attribute to the container.
		/// @param p_attribute The attribute to add.
		void add_attribute(const Ref<AttributeBase> &p_attribute);

		/// @brief Adds a buff to the container.
		/// @param p_buff The buff to add.
		void apply_buff(const Ref<AttributeBuff> &p_buff);

		/// @brief Removes an attribute from the container.
		/// @param p_attribute The attribute to remove.
		void remove_attribute(const Ref<AttributeBase> &p_attribute);

		/// @brief Removes a buff from the container.
		/// @param p_buff The buff to remove.
		void remove_buff(const Ref<AttributeBuff> &p_buff) const;

		/// @brief Sets up the container.
		void setup();

		/// @brief Finds an attribute in the container.
		/// @param p_predicate The predicate to use to find the attribute.
		/// @return The attribute found.
		[[nodiscard]] Ref<RuntimeAttribute> find(const Callable &p_predicate) const;

		/// @brief Finds an attribute buffed value in the container.
		/// @param p_predicate The predicate to use to find the attribute buffed value.
		/// @return The attribute buffed value found.
		[[nodiscard]] float find_buffed_value(const Callable &p_predicate) const;

		/// @brief Finds an attribute value in the container.
		/// @param p_predicate The predicate to use to find the attribute value.
		/// @return The attribute value found.
		[[nodiscard]] float find_value(const Callable &p_predicate) const;

		// getters/setters
		/// @brief Returns the attributes of the container.
		/// @return The attributes of the container.
		[[nodiscard]] Ref<AttributeSet> get_attribute_set() const;

		/// @brief Returns the `manual_ticking` flag.
		/// @return The flag.
		[[nodiscard]] bool get_manual_ticking() const;

		/// @brief Returns the attributes of the container.
		/// @return The attributes of the container.
		[[nodiscard]] TypedArray<RuntimeAttribute> get_runtime_attributes() const;

		/// @brief Gets an attribute by name.
		/// @param p_name The name of the attribute to get.
		/// @return The attribute with the given name.
		[[nodiscard]] Ref<RuntimeAttribute> get_runtime_attribute_by_name(const String &p_name) const;

		/// @brief Gets the buffed value of an attribute by name. It returns 0.0f if the attribute is not found. Or if the actual value is 0.0f.
		/// @param p_name The name of the attribute to get.
		/// @return The buffed value of the attribute with the given name.
		[[nodiscard]] float get_attribute_buffed_value_by_name(const String &p_name) const;

		/// @brief Gets the value of an attribute by name.
		/// @param p_name The name of the attribute to get.
		/// @return The value of the attribute with the given name.
		[[nodiscard]] float get_attribute_previous_value_by_name(const String &p_name) const;

		/// @brief Gets the base value of an attribute by name.
		/// @param p_name The name of the attribute to get.
		/// @return The base value of the attribute with the given name.
		[[nodiscard]] float get_attribute_value_by_name(const String &p_name) const;

		/// @brief Sets the attributes of the container.
		/// @param p_attribute_set The attributes to set.
		void set_attribute_set(const Ref<AttributeSet> &p_attribute_set);

		/// @brief Sets the manual_ticking bool
		/// @param p_manual_ticking True to handle manual ticking programmatically. See `AttributeContainer::set_tick(float p_tick)` function
		void set_manual_ticking(bool p_manual_ticking);

		/// @brief Subtracts the passed p_tick value to all timed transient AttributeBuff instances durations in the container.
		/// 	   Ideal for turn-based games or to handle programmatic resets/updates.
		/// @param p_tick The tick value. The value passed is subtracted from transient AttributeBuff durations
		void subtract_attribute_buffs_ticks(float p_tick);

	protected:
		/// @brief Bind methods to Godot.
		static void _bind_methods();
		/// @brief Attribute's set.
		Ref<AttributeSet> attribute_set;
		/// @brief TypedArray of attributes.
		Dictionary attributes;
		/// @brief The attribute buff context.
		Ref<AttributeBuffContext> buff_context;
		/// @brief The dictionary storing the attribute values.
		Dictionary attributes_values;
		/// @brief Derived attributes. These are attributes that are calculated from other attributes.
		Dictionary derived_attributes;
		/// @brief If set to true, AttributeBuff durations are going to be handled by you.
		bool manual_ticking = false;

		/// @brief Receives a notification.
		/// @param p_what The notification.
		// ReSharper disable once CppHidingFunction
		void _notification(int p_what);

		/// @brief Handles the attribute_changed signal.
		/// @param p_attribute The attribute that changed.
		/// @param p_previous_value The previous value of the attribute.
		/// @param p_new_value The new value of the attribute.
		void _on_attribute_changed(const Ref<RuntimeAttribute> &p_attribute, float p_previous_value, float p_new_value);

		/// @brief Handles the buff_applied signal.
		/// @param p_buff The buff that was applied.
		void _on_buff_applied(RuntimeBuff *p_buff);

		/// @brief Handles the buff_removed signal.
		/// @param p_buff The buff that was removed.
		void _on_buff_removed(const Ref<RuntimeBuff> &p_buff);

		/// @brief Handles the buff_time_updated signal.
		/// @param p_buff The buff that was updated.
		void _on_buff_time_updated(const Ref<RuntimeBuff> &p_buff);

		/// @brief Checks if the container has a specific attribute.
		[[nodiscard]] bool has_attribute(const Ref<AttributeBase> &p_attribute) const;

		/// @brief Notifies derived attributes that an attribute has changed.
		/// @param p_base_runtime_attribute The attribute that changed.
		void notify_derived_attributes(const Ref<RuntimeAttribute> &p_base_runtime_attribute);
	};
} //namespace octod::gameplay::attributes

#endif