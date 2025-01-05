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
 * \file macroaction.h
 * \brief Declaration file for an key sequence (macro) simulation class
 * \author Eduard Huguet <eduardhc@gmail.com>
 */

#ifndef __JSMAPPERLIB_MACRO_ACTION_H_
#define __JSMAPPERLIB_MACRO_ACTION_H_

#include "action.h"
#include <vector>

namespace jsmapper
{
	/**
	  \brief Macro simulation action

	  This class allows user to define a sequence of key events ("macro") to be mapped
	  to an axis / button event. For the sake of simplicity, this action type
	  will be able to send only a sequence of simple keystrokes (this is, a
	  key press event followed by a key release one), optionally featuring
	  modifiers such as Shift, Ctrl or Alt for every one of the keystrokes to
	  send.

	  The class allows to define the rate at which key events will be generated.
	  */
	class MacroAction: public Action
	{
	public:
		/// Default key spacing, in ms
		static const int DefaultSpacing;

	public:
		MacroAction( const std::string &name = std::string(),
						   bool filter = true,
						   const std::string description = std::string() );
		virtual ~MacroAction();


	// attributes:
	public:
		class Key;
		typedef std::vector<Key> KeyList;

		/**
		  \brief Adds a keystroke to the sequence
		  */
		void addKey( const Key &key );
		void addKey( uint key, uint modifiers = 0 );

		/**
		  \brief Return reference to the list of keystrokes
		  */
		const KeyList &getKeys() const;

		/**
		  \brief Clears keystroke list
		  */
		void clearKeys();

		/**
		  \brief Sets keystroke event spacing

		  Sets event key spacing interval. This is, the number of milliseconds that the driver
		  will wait between every on the assigned keystrokes.

		  Default value is 250 ms.

		  \param ms Key spacing, in ms
		  */
		void setSpacing( uint ms );

		/**
		  \brief Returns key event spacing
		  \see setKeySpacing()

		  \return Key spacing, in ms
		  */
		uint getSpacing() const;


	// serialization:
	public:
		/**
		 * \brief Saves action to an XML node and returns it
		 */
		virtual xmlNodePtr toXml() const;

		/**
		 * \brief Restores action from an XML node
		 */
		virtual bool fromXml( xmlNodePtr node );


	protected:
		/**
		  \brief Serialized keys array into an XML node
		  */
		xmlNodePtr keysToXml() const;
		
		/**
		  \brief Restores keys array into from the XML node
		  */
		void keysFromXml( xmlNodePtr nodeKeys );
		

	// loading into device:
	public:
		/**
		 * \brief Creates device action struct
		 *
		 * This function is invoked by profile loader in order to create the memory struct needed that will be sent
		 * to the driver, when loading this action into the device.
		 */
		virtual struct t_JSMAPPER_ACTION * toDeviceAction( size_t &cbBuffer ) const;

	private:
		class Private;
		Private *d;
	};


	/**
	  \brief Keystroke for MacroAction
	  */
	class MacroAction::Key
	{
	public:
		uint id;
		uint modifiers;

	public:
		Key( uint _id = 0, uint _modifiers = 0 )
			: id( _id ), modifiers( _modifiers )
		{
		}

		Key( const Key &src )
		{
			id = src.id;
			modifiers = src.modifiers;
		}
	};

}

#endif // __JSMAPPERLIB_KEYSEQUENCE_ACTION_H_
