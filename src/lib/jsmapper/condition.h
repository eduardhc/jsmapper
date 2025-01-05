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
 * \file condition.h
 * \author Eduard Huguet <eduardhc@gmail.com>
 * \brief Declaration file for Condition class and derivatives
 */

#ifndef __JSMAPPERLIB_CONDITION_H_
#define __JSMAPPERLIB_CONDITION_H_

#include "common.h"
#include <string>

namespace jsmapper
{
	/**
	 * \brief Base class for mode activation conditions
	 * 
	 * This is the base class for all activation conditions for modes. Any mode (except
	 * root mode) must have an activation condition, which specified under which circumstances
	 * it becomes active.
	 * 
	 * In order for a submode to become active, its parent mode must become it first. This 
	 * means that the same condition can be used for more than a mode, as long they don'tef 
	 * have the chance to be activated simultaneously. 
	 */
	class Condition
	{
	public:
		Condition();
		virtual ~Condition();
		
	public:
		/**
		 * \brief Saves condition to an XML node and returns it
		 */
		virtual xmlNodePtr toXml() const;
		
		static Condition * createFromXml( xmlNodePtr node );
		
		/**
		 * \brief Restores condition from an XML node
		 */
		virtual bool fromXml( xmlNodePtr node );
        
	
    public:
        /**
		  \brief Fills trigger condition-related data inside mode parameters structure
          */
        virtual bool toDeviceCondition( Device * dev, struct t_JSMAPPER_MODE * mode ) = 0;
	};
	
	
	/**
	 * \brief A button-based activation condition
	 * 
	 * When using this condition, the mode will be active while the specified button ID
	 * is held down.  
	 */
	class ButtonCondition: public Condition
	{
	public:
		ButtonCondition( const std::string &btnId = std::string() );
		virtual ~ButtonCondition();

	public:
		/**
		 * \brief Returns the button that triggers on the condition
		 */
		const std::string & getButton() const;
		
		/**
		 * \brief Sets the button that triggers on the condition
		 */
		void setButton( const std::string &btnId );
		
	
	public:
		/**
		 * \brief Saves condition to an XML node and returns it
		 */
		virtual xmlNodePtr toXml() const;

		/**
		 * \brief Restores condition from an XML node
		 */
		virtual bool fromXml( xmlNodePtr node );

	
    public:
        /**
          \brief Fills trigger member of parameter structure
          */
        virtual bool toDeviceCondition( Device * devMap, struct t_JSMAPPER_MODE * mode );


	protected:
		std::string m_btnId;
	};
}

#endif
