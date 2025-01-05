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
 * \file keymap.h
 * \brief Declaration file for key map helper class
 * \author Eduard Huguet <eduardhc@gmail.com>
 */

#ifndef __JSMAPPERLIB_KEYMAP_H_
#define __JSMAPPERLIB_KEYMAP_H_

#include "common.h"
#include <string>
#include <list>

namespace jsmapper
{
	/**
	 * \brief Helper class for key symbol mapping
	 * 
	 * This class provides name mapping for keys, buttons, etc... defined in linux/input.h file. Specifically, 
     * it provides a bidirectional name mapping for the following constants:
     * \li KEY_xxx (only first 255 values), through getKeySymbol() and getKeyId()
     * \li REL_xxx, through getRelAxisSymbol() and getRelAxisId()
     * \li BTN_xxx, through getButtonSymbol() and getButtonId()
	 */
	class KeyMap
	{
	public:
		/**
		 * \brief Returns global instance of KeyMap clas
		 */
		static KeyMap * instance();
		
	
	// keys:
	public:
        /**
          * \brief Gets the whole list of supported key symbols
          */
        std::list<std::string> getKeySymbols() const;
		
		/**
		 * \brief Returns key symbol for given key ID
		 * 
		 * Returns the key symbol (name) to use to refer to this key inside the XML
		 * profile definition file.
		 * 
		 * \param keyId Key value identifier (KEY_xxx constants from linux/input.h file)
		 * \return Symbolic name for the key, if known, an empty string otherwise 
		 */
		std::string getKeySymbol( uint keyId ) const;
	
		/**
		 * \brief Returns key ID for given key symbol
		 * 
		 * \param keySymbol Symbolic name for the key
		 * \return A KEY_xxx value ID from linux/input.h file, if known, 0 otherwise 
		 */
		uint getKeyId( const std::string &keySym ) const;

        
    // button:
    public:        
        /**
          * \brief Gets the whole list of supported button symbols
          */
        std::list<std::string> getButtonsSymbols() const;
		
		/**
		 * \brief Returns button symbol for given button ID
		 * 
		 * Returns the button symbol (name) to use to refer to this button inside the XML
		 * profile definition file.
		 * 
		 * \param btnId Button value identifier (BTN_xxx constants from linux/input.h file)
		 * \return Symbolic name for the button, if known, an empty string otherwise 
		 */
		std::string getButtonSymbol( uint btnId ) const;
	
		/**
		 * \brief Returns button ID for given button symbol
		 * 
		 * \param btnSymbol Symbolic name for the button
		 * \return A BTN_xxx value ID from linux/input.h file, if known, 0 otherwise 
		 */
		uint getButtonId( const std::string &btnSym ) const;
            
            
            
	// modifiers:
	public:
		/**
		 * \brief Returns key modifier symbol for given key modifier ID
		 * 
		 * Returns the symbol (name) to use to refer to this modifier key inside the XML
		 * profile definition file.
		 * 
		 * \param modId Modifier value identifier, from jsmapper_api.h file
		 * \return Symbolic name for the modifier, if known, an empty string otherwise 
		 */
		std::string getModifierSymbol( uint modId ) const;
	
		/**
		 * \brief Returns key ID for given key modifier symbol
		 * 
		 * \param modSymbol Symbolic name for the modifier
		 * \return A key modifier value ID from jsmapper_api.h file, if known, 0 otherwise 
		 */
		uint getModifierId( const std::string &modSym ) const;
		

		/**
		  \brief Converts a modifiers mask to string list form, suitable for use inside an XML attribute
		  */
		std::list<std::string> getModifiersSymbols( uint modifiers ) const;

		/**
		  \brief Converts a back a modifiers string list to a modifiers mask
		  */
		uint getModifiersIds( const std::list<std::string> &lstModif ) const;


    // axes:
    public:        
        /**
          * \brief Gets the whole list of supported relative axes symbols
          */
        std::list<std::string> getRelAxesSymbols() const;
        
        /**
         * \brief Returns relative axis symbol for given axis ID
         * 
         * Returns the axyis symbol (name) to use to refer to this axis inside the XML
         * profile definition file.
         * 
         * \param axisId Axis value identifier (REL_xxx constants from from linux/input.h file)
         * \return Symbolic name for the axis, if known, an empty string otherwise 
         */
        std::string getRelAxisSymbol( uint axisId ) const;
    
        /**
         * \brief Returns axis ID for given relative axis symbol
         * 
         * \param axisSymbol Symbolic name for the axis
         * \return An REL_xxx axis value ID from linux/input.h file, if known, 0 otherwise 
         */
        uint getRelAxisId( const std::string &axisSym ) const;
        
    
    protected:
		/**
		 * \brief Default ctor
		 * 
		 * Declared as 'protected', as the class is defined as a singleton and thus 
		 * there's a single one of them, permanantly available.
		 */
		KeyMap();
		virtual ~KeyMap();
		
		static KeyMap theMap;
	
		/**
		 * \brief Initializes key map
		 * Initializes the whole key map.
		 */
		void init();
		
		/**
		 * \brief Adds a key to mapping
		 * 
		 * \param keyId Key identifier from linux/input.h
		 * \param keySym Key symbolic name
		 */
		void addKey( uint keyId, const char * keySym );
		
        /**
		 * \brief Adds a button to mapping
		 * 
		 * \param btnId Button identifier from linux/input.h
		 * \param btnSym Button symbolic name
		 */
		void addButton( uint btnId, const char * btnSym );
        
		/**
		 * \brief Adds a key modifier to mapping
		 * 
		 * \param modId Key modifier identifier from jsmapper_api.h
		 * \param modSym Key modifier symbolic name
		 */
		void addModifier( uint modId, const char * modSym );
		
        /**
		 * \brief Adds a relative axis to mapping
		 * 
		 * \param relId Axis identifier (REL_xxx) from linux/input.h
		 * \param relSym Axis symbolic name
		 */
		void addRel( uint relId, const char * relSym );
        
		
	private:
		class Private;
		Private * d;
	};
}

#endif
