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
 * \file monitor.cpp
 * \brief Implementation file for device monitor class
 * \author Eduard Huguet <eduardhc@gmail.com>
 */

#include "monitor.h"
#include "device.h"
#include "log.h"

#include <libudev.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <list>

namespace jsmapper
{
	/**
	 * @brief The Monitor::Private class
	 */
	class Monitor::Private
	{
	public:
		/** Client list */
		std::list<Client *> clients;
		/** Thread object */
		pthread_t thread;
		/** Mutex object */
		pthread_mutex_t mutex;
		/** Thread status flag */
		bool running;
		/** udev context */
		struct udev * udev;
		/** udev monitor */
		struct udev_monitor * udev_mon;
		/** Monitor file descriptor */
		int udev_fd;

	public:
		Private()
			: thread( (pthread_t) -1 ),
			  running( false ),
			  udev( NULL ), 
			  udev_mon( NULL ), 
			  udev_fd( -1 )
		{
			pthread_mutex_init( &mutex, NULL );
		}

		~Private()
		{
			pthread_mutex_destroy( &mutex );
		}

		void notifyEvent( Event * event )
		{
			std::list<Client *>::const_iterator it = clients.begin();
			while( it != clients.end() )
			{
				Client * client = *it++;
				client->onEvent( event );
			}
		}
	};


	//

	Monitor::Monitor()
	{
		d = new Private();

		Log::getLog()->setLogLevel( Log::DEBG );
	}

	Monitor::~Monitor()
	{
		if( d->running )
		{
			JSMAPPER_LOG_WARNING( "Stopping monitor from destructor!" );
			stop();
		}
		
		delete d;
	}


	//
	// client handling
	//

	Monitor::Client::Client()
	{
	}

	Monitor::Client::~Client()
	{
	}


	//

	void Monitor::addClient( Client * client )
	{
		pthread_mutex_lock( &d->mutex );
		d->clients.push_back( client );
		pthread_mutex_unlock( &d->mutex );
	}

	void Monitor::removeClient( Client * client )
	{
		pthread_mutex_lock( &d->mutex );
		d->clients.remove( client );
		pthread_mutex_unlock( &d->mutex );
	}


	//
	// start / stop
	//

	bool Monitor::start()
	{
		bool ret = false;

		pthread_mutex_lock( &d->mutex );
		if( d->running == false )
		{
			// init udev & launch thread:
			if( initUdev() )
			{
				JSMAPPER_LOG_DEBUG( "Starting monitorization thread..." );
				int err = pthread_create( &d->thread, NULL, _fnMonitor, (void *) d );
				if( err == 0 )
				{
					d->running = true;
					ret = true;
				}
				else
					JSMAPPER_LOG_ERROR( "Failed to start monitorization"
									" (error %i: %s)!", errno, strerror( errno ) );
			}
			
			// clean up udev context if anything failed:
			if( ret == false )
				doneUdev();
				
		}
		else
			JSMAPPER_LOG_WARNING( "Monitorization yet started!" );

		pthread_mutex_unlock( &d->mutex );
		return ret;
	}

	//

	bool Monitor::stop()
	{
		bool ret = false;

		pthread_mutex_lock( &d->mutex );
		if( d->running )
		{
			// close udev's monitor fd - this will make thread stop:
			if( d->udev_fd > 0 )
			{
				close( d->udev_fd );
				d->udev_fd = -1;
			}
			

			// wait for thread stop:
			JSMAPPER_LOG_DEBUG( "Waiting for monitorization thread to stop..." );

			pthread_mutex_unlock( &d->mutex );
			int err = pthread_join( d->thread, NULL );
			pthread_mutex_lock( &d->mutex );

			if( err == 0 )
			{
				JSMAPPER_LOG_DEBUG( "Monitorization thread stopped" );

				d->thread = (pthread_t) -1;
				d->running = false;
				
				doneUdev();
				ret = true;
			}
			else
				JSMAPPER_LOG_ERROR( "Failed to stop monitorization"
									" (error %i: %s)!", errno, strerror( errno ) );
		}
		else
			JSMAPPER_LOG_WARNING( "Monitorization not started!" );


		pthread_mutex_unlock( &d->mutex );
		return ret;
	}


	//

	bool Monitor::initUdev()
	{
		bool ret = false;
		
		// allocate udev context:
		d->udev = udev_new();
		if( d->udev )
		{
			// allocate udev monitor:
			d->udev_mon = udev_monitor_new_from_netlink( d->udev, "udev" );
			if( d->udev_mon )
			{
				// set up filtering & allocate file descriptor for event reading:
				udev_monitor_filter_add_match_subsystem_devtype( d->udev_mon, "input", NULL );
				udev_monitor_enable_receiving( d->udev_mon );
				
				d->udev_fd = udev_monitor_get_fd( d->udev_mon );
				if( d->udev_fd > 0 )
				{
					ret = true;
				}
				else
					JSMAPPER_LOG_ERROR( "Failed to allocate udev monitor file descriptor!" );
			}
			else
				JSMAPPER_LOG_ERROR( "Failed to allocate udev monitor!" );
		}
		else
			JSMAPPER_LOG_ERROR( "Failed to allocate udev context!" );
		
		return ret;
	}
	
	//
	
	void Monitor::doneUdev()
	{
		if( d->udev_fd > 0 )
		{
			close( d->udev_fd );
			d->udev_fd = -1;
		}
		
		if( d->udev_mon )
		{
			udev_monitor_unref( d->udev_mon );
			d->udev_mon = NULL;
		}
		
		if( d->udev )
		{
			udev_unref( d->udev );
			d->udev = NULL;
		}
	}

	//

	const char * /*static*/ Monitor::ADD	= "add";
	const char * /*static*/ Monitor::REMOVE	= "remove";

	//

	void * /*static*/ Monitor::_fnMonitor( void * p )
	{
		Private * d = (Private *) p;

		JSMAPPER_LOG_INFO( "Started monitorization thread." );

		bool stop = false;
		while( !stop )
		{
			pthread_mutex_lock( &d->mutex );
			int fd = d->udev_fd;
			pthread_mutex_unlock( &d->mutex );
			if( fd > 0 )
			{
				// Check if our file descriptor has received data
				//	-> we need to use a timeout, else select() would block
				//     undefinitely when file descriptor is closed from stop() .
				fd_set fds;
				FD_ZERO( &fds );
				FD_SET( d->udev_fd, &fds );

				struct timeval tv;
				tv.tv_sec = 1;
				tv.tv_usec = 0;

				int err = select( d->udev_fd + 1, &fds, NULL, NULL, &tv );
				if( err >= 0
						&& FD_ISSET( d->udev_fd, &fds ) )
				{
					// make the call to receive the device. select() ensured that this will not block.
					struct udev_device * dev = udev_monitor_receive_device( d->udev_mon );
					if( dev )
					{
						const char * node = udev_device_get_devnode( dev );
						const char * action = udev_device_get_action( dev );
						if( node != NULL )
						{
							JSMAPPER_LOG_DEBUG( "Event node: %s", node );
							JSMAPPER_LOG_DEBUG( "Event action: %s", action );

							int id = Device::getId( node );
							if( id >= 0 )
							{
								Event ev;
								ev.id = id;
								ev.type = EVENT_NONE;
								if( strcmp( action, ADD ) == 0 )
								{
									ev.type = EVENT_DEVICE_ADDED;
								}
								else if( strcmp( action, REMOVE ) == 0 )
								{
									ev.type = EVENT_DEVICE_REMOVED;
								}
								else
									JSMAPPER_LOG_WARNING( "Unknown action: %s", action );

								pthread_mutex_lock( &d->mutex );
								d->notifyEvent( &ev );
								pthread_mutex_unlock( &d->mutex );
							}
						}

						udev_device_unref( dev );
					}
					else
						JSMAPPER_LOG_ERROR( "Failed receive_device() call (error %i: %s)",
											errno, strerror( errno ) );
				}

			}
			else
			{
				JSMAPPER_LOG_DEBUG( "Stopping monitorization loop" );
				stop = true;
			}
		}

		JSMAPPER_LOG_INFO( "Exiting monitorization thread..." );
		return NULL;
	}
}

//

