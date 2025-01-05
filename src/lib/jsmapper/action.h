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
 * \file action.h
 * \brief Declaration file for base Action class
 * \author Eduard Huguet <eduardhc@gmail.com>
 */

#ifndef __JSMAPPERLIB_ACTION_H_
#define __JSMAPPERLIB_ACTION_H_

#include "common.h"
#include <string>

namespace jsmapper
{
	/**
	 * \brief Action base class
	 * 
	 * This is the base class for all actions defined and assigned to buttons and axes. It provides
	 * the base common features, including a name and a description, and the common interface which is later
	 * used for serializing, transferring it to the driver, etc...
	 */
	class Action
	{
	protected:
		/**
		 * \brief Base constructor
		 */
		Action( ActionType type, const std::string &name, bool filter = true, const std::string description = std::string() );
	
	public:
		virtual ~Action();

		
	public:
		/**
		 * @brief Returns action type
		 * @return 
		 */
		ActionType getType() const;
		
		/**
		 * \brief Return action's name
		 */
		const std::string & getName() const;
		
		/**
		 * \brief Changes action's name
		 */
		void setName( const std::string &name );
		
		
		/**
		 * \brief Returns filtering mode for this action
		 * 
		 * \sa setFilter 
		 */
		bool filter() const;
		
		/**
		 * \brief Sets filtering mode for this action
		 * 
		 * If true, then the originating event for this action will be filtered out, so it won't reach
		 * joystick input driver. If false, the joystick driver will still receive it after the action has been 
		 * processed.
		 */
		void setFilter( bool filter = true );
		
		
		/**
		 * \brief Return action's description
		 */
		const std::string & getDescription() const;
		
		/**
		 * \brief Changes action's description
		 */
		void setDescription( const std::string &description );


	public:
		/**
		 * \brief Saves action to an XML node and returns it
		 */
		virtual xmlNodePtr toXml() const;
		
		/**
		 * \brief Restores action from an XML node
		 */
		virtual bool fromXml( xmlNodePtr  node );
		
		static Action * buildFromXml( xmlNodePtr node );
		
	
	// device loader helper:
	public:
		/**
		 * \brief Creates device action struct
		 * 
		 * This function is invoked by profile loader in order to create the memory struct needed that will be sent 
		 * to the driver, in order to load the action associated with the button into the device.
		 * 
		 * \param cbAction On output, it will receive the size of the structure created
		 * \return Pointer to the created structure to be passed to the driver. Must be freed by the caller by using free().
		 */
		virtual struct t_JSMAPPER_ACTION * toDeviceAction( size_t &cbBuffer ) const = 0;
		
		
	private:
		class Private;
		Private * d;
	};
}



#endif

