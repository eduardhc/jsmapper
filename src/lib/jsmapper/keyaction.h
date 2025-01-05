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
 * \file keyaction.h
 * \brief Declaration file for key events simulation class
 * \author Eduard Huguet <eduardhc@gmail.com>
 */

#ifndef __JSMAPPERLIB_KEY_ACTION_H_
#define __JSMAPPERLIB_KEY_ACTION_H_

#include "action.h"
namespace jsmapper
{
	/**
	 * \brief Key event simulation action
     * 
     * This action will simulate press & release events for the given key. 
	 */
	class KeyAction: public Action
	{
	public:
		/**
		 * \brief Action constructor
		 * 
		 * \sa setKey, setModifiers, setSingle
		 */
		KeyAction( const std::string& name = std::string(), 
						uint key = 0, uint modifiers = 0, 
						bool single = false, 
						bool filter = true, 
						const std::string description = std::string() );
		
		virtual ~KeyAction();
		
	public:
		/**
		 * \brief Returns action's key
		 * \sa setKey
		 */
		uint getKey() const;
		
		/**
		 * \brief Sets action's key
		 * 
		 * Sets the key code for the key to be simulated. The key codes to use are the ones supported by Linux's kernel
		 * input device system.
		 * 
		 * Key definitions can usually be found on ${LINUX_HEADERS}/input.h file.
		 */
		void setKey( uint key );

		
		/**
		 * \brief Returns action's key modifiers to use 
		 * \sa setModifiers
		 */
		uint getModifiers() const;
		
		/**
		 * \brief Sets action's key modifiers to use 
		 * 
		 * Modifiers mask can be a combination of any of the JSMAPPER_MODIFIER_xxx  constants defined in
		 * jsmapper_api.h file.
		 */
		void setModifiers( uint modifiers );

		
		/**
		 * \brief Returns true if this will be a "single event" action
		 * \sa setSingle
		 */
		bool isSingle() const;
		
		/**
		 * \brief Sets "single event" mode for the action
		 * 
		 * If true, this attribute will mean that a single key press / release event pair will be generated when the users
		 * holds the joystick button down, no matter how long is kept. If false, the driver will generate autorepetitions
		 * for the simulated key, just as if the user would be holding down a key in a real keyboard.
		 */
		void setSingle( bool set = true );
		
		
	public:
		/**
		 * \brief Saves action to an XML node and returns it
		 */
		virtual xmlNodePtr toXml() const;
		
		/**
		 * \brief Restores action from an XML node
		 */
		virtual bool fromXml( xmlNodePtr node );

		
	public:
		/**
		 * \brief Creates device action struct
		 * 
		 * This function is invoked by profile loader in order to create the memory struct needed that will be sent 
		 * to the driver, in order to load the action associated with the button into the device.
		 */
		virtual struct t_JSMAPPER_ACTION * toDeviceAction( size_t &cbBuffer ) const;


	private:
		class Private;
		Private * d;
	};
}

#endif


