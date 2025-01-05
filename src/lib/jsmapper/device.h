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
 * \file device.h
 * \author Eduard Huguet <eduardhc@gmail.com>
 * \brief Declaration file for 'Device' class
 */

#ifndef __LIBJSMAPPER_DEVICE_H_
#define __LIBJSMAPPER_DEVICE_H_

#include "common.h"
#include <string>

namespace jsmapper
{
	/**
	 * \brief JSMapper device class
	 * 
	 * This class is the main interface with the underlying jsmapper kernel device. It provides the necessary functions 
	 * to interact with the driver, query device information, status, etc..., and also the main entry point for device 
	 * programming functions.
	 */

	class Device
	{
	public:
		/** Device node prefix ("jsmap") */
		static std::string PREFIX;
		/** Device node prefix length (5) */
		static int PREFIX_LENGTH;

	public:
		/**
		 * \brief Constructs the device
		 * \param id JSMapper device number (i.e. 0 -> /dev/input/jsmap0 )
		 */
		Device( int id = 0 );
		virtual ~Device();

	// basic file operations
	public:
		/**
		 * \brief Checks if a device with this ID exists
		 */
		static bool test( int id );

		/**
		 * \brief Opens device file
		 */
		bool open();

		/**
		 * \brief Returns true if device is currently opened
		 */
		bool isOpen() const;
		
		/**
		 * \brief Closes device file
		 */
		void close();
        
        /**
          \brief Returns path for device
         */
        std::string getPath() const;
		
		/**
          \brief Returns path for device
         */
        static std::string getPath( int id );

		/**
		 * @brief Returns device minor from a /dev/input/jsmap* path
		 * @param
		 * @return Device minor if valid path, -1 otherwise
		 */
		static int getId( const std::string &path );


	// device querying:
	public:
		/**
		 * \brief Returns device name
		 * 
		 * Queries device name from the driver. The device file is opened if needed to access the device.
		 * 
		 * \return Device name if succesful, an empty string otherwise
		 */
		std::string getName();

		
		/**
		 * \brief Returns driver version
		 * 
		 * Queries module version from the driver. The device file is opened if needed to access the device.
		 * 
		 * \return Device driver version if succesful, <0 otherwise
		 */
		long getVersion();
		
		
		/**
		 * \brief Returns number of device buttons
		 */
		int getNumButtons();

        /**
          * \brief Returns current button value (0: released, 1: pressed)
          */
        int getButtonValue( ButtonID id );
        
        /**
		 * \brief Returns number of device axes
		 */
		int getNumAxes();
		
        /**
          * \brief Returns current button value (0: released, 1: pressed)
          */
        int getAxisValue( AxisID id );
        
        
	// programming functions:
	public:
		/**
		 * \brief Clears profile
		 * 
		 * Clears all button assignments, modes, axes, etc...
		 */
		bool clear();

		/**
		 * @brief Returns currently loaded profile name from device
		 * @return
		 */
		std::string getProfileName();

		/**
		 * @brief Sets loaded profile name to device
		 * @param name Profile name
		 * @return true if succesful, false otherwise
		 */
		bool setProfileName( const std::string &name );

        
        /**
          \brief Adds a new mode
          
          This function is used to create a new mode inside the device, which will become child of the given 
          parent mode, and will avctivate upon the given trigger.
          
          \return New mode ID if positive, 0 if failed.
          */
        uint addMode( Condition * condition, uint parentModeId );
        
        
        /**
          \brief Assigns button action
          */
        bool setButtonAction( uint modeId, ButtonID btnId, Action * action );
		
		/**
		  \brief Assigns axis action
		  */
		bool setAxisAction( uint modeId, AxisID axisId, const Band &band, Action * action );

	
	public:
		/**
		 * @brief Sets device map to use to convert from button & axis names to IDs
		 * @param map
		 */
		void setDeviceMap( DeviceMap * map );
		
		/**
		 * @brief Returns current device map 
		 */
		DeviceMap * getDeviceMap() const;


	private:
		/// Internal implementation class
		class Private;
		/// Pointer to internal implementation class
		Private * d;
	};
}

#endif

