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
 * \file axisaction.h
 * \brief Declaration file for axis events simulation class
 * \author Eduard Huguet <eduardhc@gmail.com>
 */

#ifndef __JSMAPPERLIB_AXIS_ACTION_H_
#define __JSMAPPERLIB_AXIS_ACTION_H_

#include "action.h"
namespace jsmapper
{
	/**
	 * \brief Axis event simulation action
     *
     * This action is used to simulate axis-based actions, such as moving the mouse. Right now, only relative 
     * axes are supported (REL_xxx constants)
	 */
	class AxisAction: public Action
	{
    public:
        /// Default axis (X)
        static const uint DefaultAxis;
        /// Default single mode (false)
        static const bool DefaultSingle;
        /// Default step value (1)
        static const int  DefaultStep;
        /// Default spacing between steps value, in ms (100)
        static const uint  DefaultSpacing;
        

	public:
		/**
		 * \brief Action constructor
		 * 
		 * \sa setAxis, setType, setStep, setSpacing
		 */
		AxisAction( const std::string& name = std::string(), 
                        uint axis = DefaultAxis, 
                        int step = DefaultStep, 
                        bool single = DefaultSingle, 
                        uint spacing = DefaultSpacing, 
						bool filter = true, 
						const std::string description = std::string() );
		
		virtual ~AxisAction();
		
	public:
		/**
		 * \brief Returns action's axis
		 * \sa setAxis
		 */
		uint getAxis() const;
		
		/**
		 * \brief Sets action's key
		 * 
		 * Sets the axis ID to be simulated. Only relative axes are supported (REL_xxx constants)
		 */
		void setAxis( uint axis );
		
        
        /**
		 * \brief Returns step value
		 * \sa setStep
		 */
		int getStep() const;
		
		/**
		 * \brief Sets step value
		 * Sets the number of axis units to be simulated for every step
		 */
		void setStep( int step );

        
        /**
		 * \brief Returns true if this will be a "single event" action
		 * \sa setSingle
		 */
		bool isSingle() const;
		
		/**
		 * \brief Sets "single event" mode for the action
		 * 
		 * If true, then axis movement will keep repeating as long as the source button is hold down. Else, only 
         * a single step event will be sent.
		 */
		void setSingle( bool set = true );

        
        /**
		 * \brief Returns spacing between steps, in ms 
		 * \sa setSpacing
		 */
		uint getSpacing() const;
		
		/**
		 * \brief Sets spacing between steps, in ms
		 */
		void setSpacing( uint spacing );
        
		
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


