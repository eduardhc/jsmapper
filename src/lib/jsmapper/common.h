/**
 * Copyright 2013 Eduard Huguet Cuadrench
 * 
 * This file is part of JSMapper Library.
 * 
 * JSMapper Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 2.1 of the License.
 * 
 * Foobar is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with JSMapper Library.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * \file common.h
 * \author Eduard Huguet <eduardhc@gmail.com>
 * \brief Common definitions and forward declarations file for JSMapper library
 */

#ifndef __LIBJSMAPPER_COMMON_H_
#define __LIBJSMAPPER_COMMON_H_

#include <jsmapper_config.h>         // cmake-generated

#include <linux/input.h>
#include <linux/drivers/input/jsmapper_api.h>

#include <libxml/tree.h>

#include <list>

namespace jsmapper
{
	// Basic types:
	typedef unsigned int ButtonID;
	typedef unsigned int AxisID;

	extern const ButtonID	INVALID_BUTTON_ID;
	extern const AxisID		INVALID_AXIS_ID;
	
	// Forward classes:
	class Device;
	class DeviceMap;
	class Monitor;
	
	class Action;
		class AxisAction;
		class KeyAction;
        class MacroAction;
		class NullAction;
		
	class Band;
		
    class Mode;
    typedef std::list<Mode *> ModeList;
	
	class Condition;
		class ButtonCondition;
	
	class Profile;
	
	// Function types:
	typedef bool (ENUMDEVICEMAPSPROC)( DeviceMap * map, void * data );
	
	
	//////////////////////////////////////////////////////////////////////////////////////////////////
	//
	// Enumerated types
	//
	//////////////////////////////////////////////////////////////////////////////////////////////////
	
	typedef enum 
	{
		/** Undefined action type */
		UnknownActionType = 0, 
		/** Axis action */
		AxisActionType = 1, 
		/** Axis action */
		ButtonActionType = 2, 
		/** Key action */
		KeyActionType = 3, 
		/** Macro action */
		MacroActionType = 4,
		/** Null action */
		NullActionType = 5
	} ActionType;
	
	
	//////////////////////////////////////////////////////////////////////////////////////////////////
	//
	// String constants used in XML storing
	//
	//////////////////////////////////////////////////////////////////////////////////////////////////
	
    // same identifiers uuse for keys:
	#define JSMAPPER_XML_MODIFIER_SHIFT_L		"LEFTSHIFT"
	#define JSMAPPER_XML_MODIFIER_SHIFT_R		"RIGTHSHIFT"
	#define JSMAPPER_XML_MODIFIER_CTRL_L		"LEFTCTRL"
	#define JSMAPPER_XML_MODIFIER_CTRL_R		"RIGHTCTRL"
	#define JSMAPPER_XML_MODIFIER_ALT_L			"LEFTALT"
	#define JSMAPPER_XML_MODIFIER_ALT_R			"RIGHTALT"
	#define JSMAPPER_XML_MODIFIER_META_L		"LEFTMETA"
	#define JSMAPPER_XML_MODIFIER_META_R		"RIGHTMETA"
	

	#define JSMAPPER_XML_TAG_DEVICE				"device"
	#define JSMAPPER_XML_TAG_PROFILE			"profile"
	#define JSMAPPER_XML_TAG_MODE				"mode"
	#define JSMAPPER_XML_TAG_CONDITION			"condition"
	#define JSMAPPER_XML_TAG_ACTION				"action"
    #define JSMAPPER_XML_TAG_ACTIONS			"actions"
    #define JSMAPPER_XML_TAG_BUTTON				"button"
	#define JSMAPPER_XML_TAG_AXIS				"axis"
	#define JSMAPPER_XML_TAG_BAND				"band"
	#define JSMAPPER_XML_TAG_LOW				"low"
	#define JSMAPPER_XML_TAG_HIGH				"high"
	#define JSMAPPER_XML_TAG_NAME				"name"
    #define JSMAPPER_XML_TAG_TARGET				"target"
	#define JSMAPPER_XML_TAG_DESCRIPTION		"description"
	#define JSMAPPER_XML_TAG_TYPE				"type"
	#define JSMAPPER_XML_TAG_ID					"id"
	#define JSMAPPER_XML_TAG_FILTER				"filter"
	#define JSMAPPER_XML_TAG_SINGLE				"single"
	#define JSMAPPER_XML_TAG_KEY				"key"
	#define JSMAPPER_XML_TAG_KEYS				"keys"
	#define JSMAPPER_XML_TAG_MODIFIERS			"modifiers"
    #define JSMAPPER_XML_TAG_STEP               "step"
	#define JSMAPPER_XML_TAG_SPACING			"spacing"

	#define JSMAPPER_XML_TYPE_KEY				"key"
	#define JSMAPPER_XML_TYPE_MACRO             "macro"
    #define JSMAPPER_XML_TYPE_AXIS				"axis"
	#define JSMAPPER_XML_TYPE_BUTTON			"button"
	#define JSMAPPER_XML_TYPE_NONE				"none"
}

#endif

