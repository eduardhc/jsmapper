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
 * \file monitor.h
 * \brief Declaration file for device monitor class
 * \author Eduard Huguet <eduardhc@gmail.com>
 */

#ifndef __JSMAPPERLIB_MONITOR_H
#define __JSMAPPERLIB_MONITOR_H

#include "common.h"

namespace jsmapper
{
	/**
	 * @brief Device monitor for JSMapper
	 *
	 * This class implements an udev-based monitor for JSMapper devices. It
	 * will notify the attached client objects everytime a jsmapper device
	 * is either connected or disconnected.
	 *
	 * Clients can be attached to the monitor by using the addClient() function.
	 * By doing so, they will receive notifications upon devices being
	 * plugged and unplugged.
	 */
	class Monitor
	{
	public:
		/**
		 * @brief Class constructor
		 */
		Monitor();
		virtual ~Monitor();


	public:
		/**
		 * @brief Type of device events notified by the monitor
		 */
		typedef enum
		{
			EVENT_NONE = 0,
			EVENT_DEVICE_ADDED,
			EVENT_DEVICE_REMOVED
		} EventType;


		/**
		 * @brief Event structure holding information about the event
		 */
		typedef struct t_EVENT
		{
			/** Event type */
			EventType type;
			/** Device ID (minor number) */
			int id;
		} Event;


		/**
		 * @brief Virtual client class for Monitor
		 *
		 * Receives device connection & disconnection events from the monitor.
		 */
		class Client
		{
		public:
			/**
			 * @brief Default empty constructor
			 */
			Client();
			virtual ~Client();

		public:
			/**
			 * @brief Client event handler
			 * @param event The event being produced
			 *
			 * This function must be overriden by derived classes. The
			 * implementation should notify the UI in a non-blocking way,
			 * and be aware that it gets called from a worker tread.
			 */
			virtual void onEvent( Monitor::Event * event ) = 0;
		};


	public:
		/**
		 * @brief Attached a client to the monitor
		 * @param client
		 *
		 * The client is NOT owned by the Monitor class, so it won't destroy
		 * it.
		 */
		void addClient( Client * client );

		/**
		 * @brief Detaches a client from the monitor
		 * @param client
		 */
		void removeClient( Client * client );


	public:
		/**
		 * @brief Starts monitorization thread
		 * @return true if successful, false otherwise
		 */
		bool start();

		/**
		 * @brief Stops monitorization thread
		 * @return true if successful, false otherwise
		 */
		bool stop();

	
	private:
		/**
		 * @brief Initialized udev monitoring
		 * @return true if successful, false otherwise
		 */
		bool initUdev();
		/**
		 * @brief Finishes udev monitoring
		 */
		void doneUdev();
		
		/**
		 * @brief Monitorization thread's main function
		 * @return
		 */
		static void * _fnMonitor( void * );

		/** Device "add" action name */
		static const char * ADD;
		/** Device "remove" action name */
		static const char * REMOVE;

	private:
		class Private;
		Private *d;
	};
}

#endif // __JSMAPPERLIB_MONITOR_H
