/**
 * Copyright 2013 Eduard Huguet Cuadrench
 * 
 * This file is part of JSMapper kernel module.
 * 
 * JSMapper is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2.1 of the License.
 * 
 * Foobar is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with JSMapper.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * \file jsmapper_core.h
 * \brief JSMapper kernel module mapping core declaration
 * \author Eduard Huguet <eduardhc@gmail.com>
 */

#ifndef __JSMAPPER_CORE_H_
#define __JSMAPPER_CORE_H_

#include <linux/input.h>
#include <linux/list.h>

#include "jsmapper_api.h"

struct jsmapdev_core_key;
struct jsmapdev_core_button_action;
struct jsmapdev_core_mode;

/********************************************************************************************************
 * 
 * Structure definitions
 * 
 ******************************************************************************************************/

/**
 * \brief Core info associated with a jsmapper device
 * 
 * This structure contains all data members related to core device functionality. This involves both 
 * some info gathered from the device itself (number of buttons, etc...), plus all the associated
 * data involving device programming, mapping modes, etc...
 * 
 * A pointer to a jsmapdev_core structure is obtained through a call to jsmapdev_core_init() function, which will 
 * allocate the structure and fill it withe initial data. This is usually performed when connect() function is called
 * from input manager.
 * 
 * The pointer must be freed when destroying the device by calling jsmapdev_core_done(), which will free up any associated
 * data members.
 *
 * \todo Add mutex to protect from concurrent access!
 */

struct jsmapdev_core {
	/** Pointer to actual input device, for convenience */
	struct input_dev * dev;
	/** Current profile name, if any */
	char * profile_name;
	/** Number of buttons actually found in device */
	int button_count;
	/** Maps from input key ID to a button index */
	uint button_map[KEY_MAX - BTN_MISC + 1];
	/** Maps from button index to input key ID */
	uint button_rmap[KEY_MAX - BTN_MISC + 1];
	/** Number of axes actually found in device */
	int axis_count;
	/** Maps from input axis ID to an axis index */
	uint axis_map[ABS_CNT];
	/** Maps from axis index to input axis ID */
	uint axis_rmap[ABS_CNT];
	/** Pointers to currently active action, for every axis */
	struct jsmapdev_core_axis_action * current_axis_action[ABS_CNT];
	/** Pointer to root mode */
	struct jsmapdev_core_mode * root_mode;
    /** Last mode ID value assigned when creating a new mode */
    uint last_mode_id;
};


/**
 * \brief Internal struct representing a generated 'key' event
 * 
 * This structure mimics the API-related JSMAPPER_KEY type.
 */

struct jsmapdev_core_key {
	/** A key code, as defined in linux/input.h header file */
	uint id;
	/** A modifier mask, as defined in API header file */
	uint modifiers;
    /** If 0, key will be hold down as long as button is pressed. Else, it will be immediately released */
    uint single;
};


/**
 * \brief Internal struct representing a generated 'relative axis' movement
 * 
 * This structure mimics the API-related JSMAPPER_REAL type.
 */

struct jsmapdev_core_rel {
	/** A relative axis ID */
	uint id;
    /** If 0, axis movement will be repeated until button is released. If 1, a single step will be sent */
    __u16 single;
    /** Number of axis units to move per step*/
	int step;
    /** Time spacing between steps, in ms (only if single=0) */
    uint  spacing;
};


/**
 * \brief Internal struct representing a macro
 */

struct jsmapdev_core_macro {
    /** time spacing between keys, in ms */
    uint spacing;
    /** number of keys contained in array */
    uint count;
    /** Pointer to array containing the key sequence (macro) to issue */
    struct jsmapdev_core_key * keys;
};



/**
 * \brief Internal struct representing an action to be triggered
 *
 * This structure contains the definition of an action to be triggered
 * on button / axis band activation.
 *
 * Structures of jsmapdev_core_action must be initialized by calling
 * jsmapper_core_init_action() function prior to utilisation, and destroyed
 * using jsmapper_core_clear_action once finished.
 */

struct jsmapdev_core_action {
	/** Action type - see JSMAPPER_ACTION_xxx constants in API header*/
	int type;
	/** Variable union struct containing action data */
	union {
		/** action struct used for 'key' action type */
		struct jsmapdev_core_key key;
        /** action struct used for 'rel' action type */
		struct jsmapdev_core_rel rel;
        /** action struct used for 'macro' */
        struct jsmapdev_core_macro macro;
	};
};


/**
 * \brief Internal struct representing an action for a button
 * 
 * This structure contains the definition of an action to be triggered on button activation, this means, when 
 * the button is pressed / released.
 *
 * The 'jsmapdev_core_mode' structure contains an array of these structures, one for every button found 
 * in the device.
 */

struct jsmapdev_core_button_action {
	/** The structure containing the action to be triggered */
	struct jsmapdev_core_action action;
	/** If true, the button event will be filtered out. Else, it will make its way to the joystick driver */
	bool filter;
};


/**
  * \brief Internal struct defining axis action bands
  *
  * This structure contains the list of action bands defined for a given axis, either of it featuring an 
  * associated action.
  *
  * The 'jsmapdev_core_mode' structure contains an array of these structures, one for every axis found 
  * in the device.
  */

struct jsmapdev_core_axis_actions {
	/** Axis bands list */
    struct list_head action_list;
};


/**
  * \brief Internal struct defining an axis band action definition
  *
  * This structure contains the action to be executed when the axis enter / exits the band.
  *
  * \todo Allow to have separate actions for band enter / exit events
  */

struct jsmapdev_core_axis_action {
	/** List item for this band, used for insertion inside axis's actions list */
    struct list_head child_item;
	/** Lower band value */
    int band_low;
    /** Upper band value */
    int band_high;
	/** The structure containing the action to be triggered */
	struct jsmapdev_core_action action;
	/** If true, the axis event will be filtered out. Else, it will make its way to the joystick driver */
	bool filter;
};


/**
 * \brief Internal struct defining a mode
 * 
 * This structure defines a full operating mode for the device. It contains the following
 * data members:
 * @li Mode ID: a numeric value identifying the mode (0 for root mode)
 * @li Parent mode ID: a numeric value identifying parent mode, if any (will be set to 0 for root mode)
 * @li Trigger condition: the condition ( button pressed, etc...) which triggers the mode.
 * @li Button actions: the size of the array will be the number of buttons detected in the device.
 */

struct jsmapdev_core_mode {
    /** Pointer to parent mode, if any */
    struct jsmapdev_core_mode * parent;
    /** List item for this mode, used for insertion inside parent's chidren list */
    struct list_head child_item;
    /** Mode ID */
    uint mode_id;
    /** Trigger type */
    uint condition_type;
    /** Union containing trigger condition data, to be interpreted according to conditionType field */
    union 
    {
        /** Trigger data for 'button' type trigger: */
        struct 
        {
			uint id;
        } button;
        /** Trigger data for 'axis' type trigger: */
        struct 
        {
			uint id;
			int low;
			int high;
        } axis;
    } condition;
    
	/** Button action array */
	struct jsmapdev_core_button_action * buttons;
	
	/** Axes bands array */
	struct jsmapdev_core_axis_actions * axes;
    
    /** Children modes list */
    struct list_head children_list;
};



/********************************************************************************************************
 * 
 * Main core functions
 * 
 ********************************************************************************************************/

/**
 * \brief Allocates a jsmapdev_core structure for the given device
 * 
 * This function will allocate & initialize a core structure for the given input device. This means mainly 
 * gathering from the device the number of buttons & axes if offers, build up the button keymap, etc...
 * 
 * @param dev Pointer to the input device just connected
 * @return A pointer to a core structure if succesful, NULL otherwise
 */

struct jsmapdev_core * jsmapper_core_init( struct input_dev * dev );


/**
 * \brief Clears all current actions, modes, etc...
 * 
 * This function will reset the core to its default state, so no actions are made to any button nor axe
 * 
 * @param core Pointer to core structure to reset
 * @param reset If 1, a new, empty root mode will be allocated. If 0, it won't.
 */
void jsmapper_core_clear( struct jsmapdev_core * core, int reset );


/**
 * \brief Destroys a jsmapdev_core structure
 * 
 * This function destroys a core structure allocated by a previous call to jsmapper_core_init(). The function will 
 * take care of releasing any internal data it has, such as modes, etc..., so all allocated memory is properly freed.
 * 
 * \warning The core pointer passed as parameter should not be used after calling this function!
 * 
 * @param core Pointer to core structure to destroy
 */

void jsmapper_core_done( struct jsmapdev_core * core );

/**
 * @brief Sets profile name into core
 * @param profile_name Pointer to new profile name (might be NULL)
 *
 * The pointer passed is owned by the core after calling this function, so it
 * will get freed automatically.
 */
void jsmapper_core_set_profile_name( struct jsmapdev_core * core, char * profile_name );

/**
 * @brief Returns current profile name
 * @return Pointer to profile name (might be NULL)
 */
char * jsmapper_core_get_profile_name( struct jsmapdev_core * core );


/********************************************************************************************************
 * 
 * Mode functions
 * 
 ********************************************************************************************************/

/**
  \brief Adds a new mode to core 
  
  This function gets called from the main IOCTL implementation. It takes care of internally creating the new mode, 
  based on the parameters passed. The 'mode' structure passed should specify a valid parentmode_id field (0 for root), 
  and a valid trigger condition.
  
  On exit, and if succesful, the mode_id member of the 'mode' structure will contain the ID for the newly created
  mode.
  
  \param core Pointer to driver core structure
  \param mode_p Pointer to new mode parameters (IMPORTANT: must converted from user to kernel space )
  \return 0 if succesful, an error code if not
  */
int jsmapper_core_add_mode( struct jsmapdev_core * core, struct t_JSMAPPER_MODE  * mode_p );


/**
  \brief Initializes a core mode structure
  */
struct jsmapdev_core_mode * jsmapper_core_init_mode( struct jsmapdev_core * core );


/**
  \brief Finds a mode given its ID, starting search at given root mode
  */
struct jsmapdev_core_mode * jsmapper_core_find_mode( struct jsmapdev_core_mode * root, uint mode_id );


/**
  \brief Checks mode trigger for activation
  
  This function will check if device state matches mode trigger, and will return != 0 if so.
  */
int jsmapper_core_mode_is_active( struct jsmapdev_core * core, struct jsmapdev_core_mode * mode );


/**
  \brief Destroys a core mode structure, including all its submodes
  */
void jsmapper_core_clear_mode( struct jsmapdev_core * core, struct jsmapdev_core_mode * mode );


/********************************************************************************************************
 *
 * Action functions
 *
 ********************************************************************************************************/

/**
 * \brief Properly initializes an action structure
 *
 * This function ensures that all fields inside the structure are properly initialized for its usage by the
 * core functions. It always should be called always when declaring a jsmapdev_core_action variable.
 *
 * @param action Pointer to the action to initialize
 */

void jsmapper_core_init_action( struct jsmapdev_core_action * action );

/**
 *  \brief Copies an action	into another
 *
 * This function is called everytime an action must be copied onto another one, in order to properly
 * duplicate the external data in case is needed.
 *
 * @param from Source action structure
 * @param to Target action structure, which should had been initialized using jsmapper_core_init_action()
 * @param 0 if succesful, a negative number indicating an error code otherwise
 */

int jsmapper_core_copy_action( const struct jsmapdev_core_action * from,  struct jsmapdev_core_action * to );

/**
 *  \brief Clears an action	struct
 *
 * This function is called everytime an action must be cleared, in order to properly free the external data
 * in case is needed, so no memory leaks are left.
 *
 * @param action Action structure to clear
 */

void jsmapper_core_clear_action( struct jsmapdev_core_action * action );


/********************************************************************************************************
 * 
 * Button action functions
 * 
 ********************************************************************************************************/

/**
 *  \brief Sets action for a button in a given mode
 * 
 * This function changes the action for a given button and mode. The action data is properly copied into the target mode, 
 * so it's safe to destroy the argument variable using jsmapper_core_clear_button_action() after calling this function.
 * 
 * @param core Pointer to the device's core structure
 * @param button_id ID of the button to assign, in the range 0..numButtons - 1
 * @param mode_id ID of the mode to change. Not used right now, shoud be zero.
 * @param assign Pointer to the action to set.
 * @param 0 if succesful, a negative number indicating an error code otherwise
 */

int jsmapper_core_set_button_action( struct jsmapdev_core * core, uint button_id, uint mode_id, const struct jsmapdev_core_button_action * assign );


/**
 *  \brief Gets current action for a button in a given mode
 * 
 * This function retrieves the current action for the given button and mode. Current action data is properly copied from 
 * the internal structures to the provided assign struct, so jsmapper_core_clear_button_action()() should be called over it
 * after calling this function and using the returned data.
 * 
 * @param core Pointer to the device's core structure
 * @param button_id ID of the button to check, in the range 0..numButtons - 1
 * @param mode_id ID of the mode to check. Not used right now, shoud be zero.
 * @param assign Pointer to the action variable that will receive the data.
 * @param 0 if succesful, a negative number indicating an error code otherwise
 */

int jsmapper_core_get_button_action( struct jsmapdev_core * core, uint button_id, 
										uint mode_id, struct jsmapdev_core_button_action * assign );


/**
 * \brief Searches for the action to apply to a given button
 * 
 * This function will use current core programming and device state to determine which action should be 
 * applied to the goven button. If the button is currently unassigned, it will return NULL.
 *
 * The function will start by recursively looking into child modes of the mode provided (if they are active): 
 * if any of them maps the button, then that action is returned. It not, then mode's own mapping is checked. 
 * The function is called initially from main module on root mode, which is always considered active.
 *
 * The submodes list is iterated backwards, so modes added later can override mappings defined by previous child modes.
 * 
 * @param core Pointer to the core structure containing programming schema
 * @param mode Mode to start search from
 * @param button_id Key identifier received from input core
 * @return A pointer to the action to use, or NULL if button is currently unassigned
 */

struct jsmapdev_core_button_action * jsmapper_core_find_button_action( struct jsmapdev_core * core, 
                                                                        struct jsmapdev_core_mode * mode, 
																		int button_id );


/********************************************************************************************************
 * 
 * Axis action functions
 * 
 ********************************************************************************************************/

/**
 * \brief Initializes an axis actions structure
 * 
 * This function ensures that all fields inside the structure are properly initialized for its usage by the 
 * core functions. It always should be called always when declaring a jsmapdev_core_axis_actions variable.
 * 
 * @param actions Pointer to the structure to initialize
 */

void jsmapper_core_init_axis_actions( struct jsmapdev_core_axis_actions * actions );

/**
 *  \brief Clears an axis actions struct
 * 
 * This function is called everytime an axis actions structure must be cleared, in order to properly free the external data
 * in case is needed, so no memory leaks are left.
 * 
 * It destroys all the actions in list by calling jsmapper_core_clear_axis_action(), then clears the list.
 * 
 * @param actions Assignment structure to clear
 */

void jsmapper_core_clear_axis_actions( struct jsmapdev_core_axis_actions * actions );


/**
 *  \brief Sets an action for an axis in a given mode
 *
 * This function adds a action for a given button and mode. Th action will be triggered whenever
 * the axis value enter the band defined in it.
 *
 * If an action matching the same band definition for this axis is found, it will be overwritten.
 * Else, a new one will be added. Also, if new action's band overlaps another, then behaviour
 * is undefined.
 *
 * The action data is properly copied into the target mode, so it's safe to destroy
 * the argument variable using jsmapper_core_clear_axis_action() after calling this function.
 *
 * @param core Pointer to the device's core structure
 * @param axis_id ID of the axis to assign, in the range 0..numAxes - 1
 * @param mode_id ID of the mode to change. Not used right now, shoud be zero.
 * @param assign Pointer to the action to set.
 * @param 0 if succesful, a negative number indicating an error code otherwise
 */

int jsmapper_core_set_axis_action( struct jsmapdev_core * core, uint axis_id, uint mode_id, const struct jsmapdev_core_axis_action * assign );


/**
 * \brief Searches for the action to apply to a given axis value change
 * 
 * This function will use current core programming and device state to determine which action should be 
 * applied for a given axis operation, by searching over the bands defined for it, if any. If no matching 
 * band is found for current axis value, it will return NULL.
 * 
 * The function will start by recursively looking into child modes of the mode provided (if they are active): 
 * if any of them maps the axis value, then that action is returned. It not, then mode's own mapping is checked. 
 * The function is called initially from main module on root mode, which is always considered active.
 * 
 * The submodes list is iterated backwards, so modes added later can override mappings defined by previous child modes.
 *
 * @param core Pointer to the core structure containing programming schema
 * @param mode Mode to start search from
 * @param axisId Key identifier received from input core
 * @param value Current axis value received from input core
 * @return A pointer to the action to use, or NULL if no action is mapped for this axis / value
 */

struct jsmapdev_core_axis_action * jsmapper_core_find_axis_action(struct jsmapdev_core * core, 
                                                                  struct jsmapdev_core_mode *mode, 
                                                                  int axis_id, int value );



/********************************************************************************************************
 * 
 * Helper functions
 * 
 ********************************************************************************************************/

/**
 * \brief Maps an input key code to a button ID
 * 
 * @param code Key code as sent by input layer
 * @return Button ID in the range 0..numButtons-1 if a valid key code, -1 otherwise
 */
int jsmapper_core_map_button( struct jsmapdev_core * core, int code );

/**
 * \brief Maps a button ID to an input key code
 * 
 * @param code Button ID in the range 0..numButtons-1
 * @return Key code as sent by input layer valid, -1 otherwise
 */
int jsmapper_core_rmap_button( struct jsmapdev_core * core, int code );


/**
 * \brief Maps a input axis code to an axis ID
 *
 * @param code Axis code as sent by input layer, in the range 0..ABS_CNT
 * @return Axis ID in the range 0..numAxis-1 if a valid axis code, -1 otherwise
 */
int jsmapper_core_map_axis( struct jsmapdev_core * core, int code );

/**
 * \brief Maps an axis ID to an input axis code
 * 
 * @param code Axis ID in the range 0..numAxes-1
 * @return Axis ID as sent by input layer valid, -1 otherwise
 */
int jsmapper_core_rmap_axis( struct jsmapdev_core * core, int code );

#endif

