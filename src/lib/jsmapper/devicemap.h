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
 * \file devicemap.h
 * \author Eduard Huguet <eduardhc@gmail.com>
 * \brief Declaration file for 'DeviceMap' class
 */

#ifndef __LIBJSMAPPER_DEVICEMAP_H_
#define __LIBJSMAPPER_DEVICEMAP_H_

#include "common.h"
#include <string>

namespace jsmapper
{
	/**
	 * \brief JSMapper device map class
	 * 
	 * This class provides name mapping for the elements (axis & buttons) featured by a device.
	 * The mapping is stored into an external XML definition file.
	 */

	class DeviceMap
	{
	public:
		/**
		 * @brief Empty class constructor
		 */
		DeviceMap( const std::string & name = std::string() );
		
		/**
		 * @brief Device constructor
		 * 
		 * Calls init() after constructing the class.
		 */
		DeviceMap( Device * dev );

		/**
		 * @brief Destructor
		 */		
		virtual ~DeviceMap();
		
	
	public:
		/**
		 * @brief Returns target device name
		 * @return 
		 */
		const std::string & getName() const;
		
		/**
		 * @brief Returns device map path
		 * @return 
		 */
		const std::string & getPath() const;

	public:
        /**
          * \brief Initializes mapping for device buttons & axes
          *
          * This function can be called to initialize button & axis names from the device. All 
          * elements detected will get a default name that will be saved when calling save().
          */
        bool init( Device * dev );

        /**
          * \brief Clears device mapping 
          * 
          * Clears device mapping, removing all associations between names & device elements.
          */
        void clear();
            
        
		/** 
		  \brief Loads map file for device
		  
		  This function will load a map file for the intended target device. This file contains 
		  a mapping between "natural" names for buttons and axis and their respective IDs in device module, 
		  so the profile will use those names at both load & saving time in order to identify the device items.

          \param file Device map file to load. 
		  */
        bool load( const std::string &file );


        /** 
		  * \brief Saves device map to file
		  * 
		  * This function will store mapped device buttons & axis, along with their names, into an XML node. 
		  */
        bool save( const std::string &file );
        

	public:
		/**
		 * @brief Enumerates all available device maps.
		 * 
		 * The callback function will get called until it returns 'false'.
		 * 
		 * @param fn Callback function. 
		 * @param data
		 */
		static void enumerate( ENUMDEVICEMAPSPROC * fn, void * data );
		
        /**
          * \brief Finds appropiate map file to load for device
          *
          * This function tries to find the appropiate map file to use for the device, by querying
          * device name.
          * 
          * \return Path to map file to use, or an empty string if none found
          */
        static std::string find( Device * dev );


        /**
          * \brief Finds appropiate map file to load for device
          *
          * \param name Device name to look for
          * \return Path to map file to use, or an empty string if none found
          */
        static std::string find( const std::string &name );

        
    public:        
		/**
		 * \brief Loads map data from an XML node
		 */
		virtual bool mapFromXml( xmlNodePtr node );
		
        /**
		 * \brief Saves map data into an XML node
		 */
		virtual xmlNodePtr mapToXml();
        
		
	public:
		/**
		  * \brief Returns button name for given ID
		  * If ID passed is not known, then an empty string is returned.
		  */
		std::string getButtonName( ButtonID id ) const;
		
        /**
		  \brief Assigns button name to given button ID
          
          If an empty name is passed, then button it's removed from mapping
		  */
		void setButtonName( ButtonID id, const std::string &name );
        
		/**
		  \brief Returns button ID for given name
		  If name passed is not known, then INVALID_BUTTON_ID is returned
		  */
		ButtonID getButtonID( const std::string &name ) const;
                
		
		/**
		  \brief Returns axis ID for given name
		  If name passed is not known, then INVALID_AXIS_ID is returned
		  */
		AxisID getAxisID( const std::string &name ) const;
		
		/**
		  \brief Returns axis name for given ID
		  If ID passed is not known, then an empty string is returned 
		  */
		std::string getAxisName( AxisID id ) const;
		
        /**
		  \brief Assigns axis name to given axis ID
          
          If an empty name is passed, then axis it's removed from mapping
		  */
		void setAxisName( AxisID id, const std::string &name );
		
		/**
          * \brief Figures name for button
          * 
          * This function tries to figure an appropiate name for the button. Right now, it just returns 
          * an stringized form of the button ID.
          * 
          * \todo Figure a way to determine if a button is FIRE, TRIGGER, etc..., if such a thing is possible
          */
        std::string figureButtonName( ButtonID id );

        /**
          * \brief Figures name for axis
          * 
          * This function tries to figure an appropiate name for the axis. Right now, it just returns 
          * an stringized form of the axis ID.
          * 
          * \todo Figure a way to determine if an axis is X, Y, RUDDER, etc..., if such a thing is possible
          */
        std::string figureAxisName( AxisID id );

		
	private:
		/** Internal implementation class */
		class Private;
		/** Pointer to internal implementation class */
		Private * d;
	};
}

#endif // __LIBJSMAPPER_DEVICEMAP_H_
