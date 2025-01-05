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
 * \file jsmapper_evgen.h
 * \brief JSMapper kernel module event generator interface
 * \author Eduard Huguet <eduardhc@gmail.com> 
 */

#ifndef __JSMAPPER_EVGEN_H_
#define __JSMAPPER_EVGEN_H_

#include <linux/types.h>
#include "jsmapper_core.h"

struct input_dev;

/**
 * \brief Initializes the virtual event generator
 */

int jsmapper_evgen_init( void );

/**
 * \brief Returns the pointer to the virtual event generator
 */

struct input_dev * jsmapper_evgen_device( void );


/**
 * \brief Simulates action
 * 
 * \param action Action to generate
 * \param press 1 if action is being activated, 0 if deactivated
 */

int jsmapper_evgen_send_action( struct jsmapdev_core_action * action, int press );


/**
 * \brief Generates a key event
 * 
 * This function will simulate the appropiate key event, given key parameters and source button status. 
 * 
 * For single-type key actions, it will send both a key press & release events whenever the button is pressed, 
 * and nothing for button release.
 *
 * For non-single key actions, it will send a key press event when the button is pressed, and a key release one 
 * when the button is released, so keyboard handler will automatically generate autorepeat, just as if a real 
 * key had been pressed.
 * 
 * \param key Pointer to the key to send, including the modifiers (Ctrl/Shift, etc...) to apply.
 * \param press 1 for source button press, 0 for release
 */

int jsmapper_evgen_send_key( struct jsmapdev_core_key * key, int press);


/**
 * \brief Generates a relative axis movement
 * 
 * This function will simulate a relative axis movement. Depending on the action type (single, etc...), it will 
 * instantly send it, or else will start / stop a background thread that keeps sending as long as the source button
 * is pressed.
 *
 * \param rel Pointer to the relative axis struct to send.
 * \param press 1 for source button press, 0 for release
 */

int jsmapper_evgen_send_rel( struct jsmapdev_core_rel * rel, int press );


/**
  * \brief Generates a sequence of key press & release events (macro)
  * 
  * This function generates a "macro" action, by sending a key press / release pair of events 
  * for everyone of the given keys, spacing every key event by the gicven amount of time.
  *
  * The keys to sent are queued into a "workqueue", so this function will return immediately while 
  * the keys are being sent in background, by calling jsmapper_evgen_send_single_key() repeteadly.
  *
  * \param macro Pointer to an structure defining the macro to send
  * \param press 1 for source button press, 0 for release
  */
int jsmapper_evgen_send_macro( struct jsmapdev_core_macro * macro, int press );



/**
 * \brief Destroys the virtual event generator
 */

void jsmapper_evgen_done( void );


#endif
