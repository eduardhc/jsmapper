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
 * \file profile.h
 * \brief Declaration file for Profile class
 * \author Eduard Huguet <eduardhc@gmail.com>
 */

#ifndef __JSMAPPERLIB_PROFILE_H_
#define __JSMAPPERLIB_PROFILE_H_

#include "common.h"

#include <string>
#include <list>

namespace jsmapper
{
	/**
	 * \brief Profile class
	 * 
	 */
	class Profile
	{
	public:
		/**
		 * \brief Profile constructor
         *
         * \param target Target device name. In a future, it might be used to check if profile is being 
         *        uploaded to the right device, etc...
		 */
		Profile( const std::string &target = std::string() );
		virtual ~Profile();
		
        /**
		  \brief Sets profile's target device name
		  */
		void setTarget( const std::string &target );
        
		/**
		  \brief Returns profile's target device name
		  */
		const std::string & getTarget() const;
		
		
	public:
		/**
		 * \brief Clears the profile
		 * 
		 * Clears the profile, by removing all submodes and reverting root mode assignments
		 * to their defaults.
		 */
		void clear();
		
		
		/**
		  \brief Sets profile's name
		  */
		void setName( const std::string &name );
		
		/**
		  \brief Returns profile's name
		  */
		const std::string & getName() const;
		
		/**
		  \brief Sets profile's description
		  */
		void setDescription( const std::string &desc );
		
		/**
		  \brief Returns profile's description
		  */
		const std::string & getDescription() const;
		
		
    // action list
    public:
        /**
          \brief Adds a new action to the action profile list

          Modes will later bind a device element to this action using its name.

          \warning If an action with the same name exists, it will be overwritten.
          */
        void addAction( Action * action );

		/**
		 * \brief Returns action names list
		 * @return 
		 */
		std::list<std::string> getActionNames() const;
		
        /**
          \brief Gets reference to action through its name

          If the action is not found, then NULL is registered
          */
        Action * getAction( const std::string &name ) const;

        /**
          \brief Removes an action through its name

          Before calling this function, any reference to it in any mode should be cleared first.
          */
        void removeAction( const std::string &name );

    public:
		/**
		 * \brief Returns a pointer to profile root mode
		 * 
		 * This function returns a pointer to the profile's root mode,which is
		 * always defined, and which contains the default assignments for any 
		 * button / axis.     
		 * 
		 * All profile submodes must be created as children of this root mode.
		 * 
		 * The pointer returned shouldn't be destroyed, as it belongs to the profile
		 * object.
		 * 
		 * \return Pointer to profile's root mode
		 */
		Mode * getRootMode() const;
		
		/**
		 * \brief Sets profile's root mode
		 * 
		 * This function assigns the root mode for the profile. Previous one is destroyed, along all its 
		 * child submodes.
		 * 
		 * \param rootMode Pointer to new root mode
		 */
		void setRootMode( Mode * rootMode );
		
	
	// serialization
	public:
		/**
		 * \brief Loads a profile from an XML file
		 */
		bool load( const std::string &file );
		
		/**
		 * \brief Saves profile to file using XML
		 */
		bool save( const std::string &file ) const;

		
		/**
		 * \brief Saves profile to an XML node and returns it
		 */
		virtual xmlNodePtr toXml() const;
		
		/**
		 * \brief Loads profile from to an XML node
		 */
		virtual bool fromXml( xmlNodePtr node );
		
        
	protected:
		/**
		  \brief Saves actions list to an XML node
		  */
		xmlNodePtr actionsToXml() const;

		/**
		  \brief Loads actions list to an XML node
		  */
		bool actionsFromXml( xmlNodePtr node );


	// loading into device:
	public:
        /**
		 * \brief Loads profile into device
		 * 
		 * This function will load all profile mappings into the device. It will clear the device first, then load 
         * all the modes, mappings, etc... into the device.
		 */
        bool toDevice( Device * dev );
        
        
	private:
		class Private;
		Private * d;
	};	
}

#endif
