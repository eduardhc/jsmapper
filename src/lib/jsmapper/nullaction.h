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
 * \file nullaction.h
 * \brief Declaration file for empty action
 * \author Eduard Huguet <eduardhc@gmail.com>
 */

#ifndef __JSMAPPERLIB_NULL_ACTION_H_
#define __JSMAPPERLIB_NULL_ACTION_H_

#include "action.h"
namespace jsmapper
{
	/**
	 * \brief Empty action
	 * 
	 * This action is used only to "disable" a button / axis on the device, by mapping it to this action 
	 * (which does nothing) and setting filter flag to true.
	 */
	class NullAction: public Action
	{
	public:
		NullAction( const std::string& name = std::string(), 
						bool filter = true, 
						const std::string description = std::string() );
		
		virtual ~NullAction();
		
		
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
		 * to the driver.
		 */
		virtual struct t_JSMAPPER_ACTION * toDeviceAction( size_t &cbBuffer ) const;
	};
}

#endif
