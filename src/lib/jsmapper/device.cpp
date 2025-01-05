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
 * \file device.cpp
 * \author Eduard Huguet <eduardhc@gmail.com>
 * \brief Implementation file for 'Device' class
 */

#include "device.h"
#include "devicemap.h"
#include "log.h"
#include "profile.h"
#include "mode.h"
#include "action.h"
#include "condition.h"
#include "xmlhelpers.h"
#include "band.h"

#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>


using namespace std;

namespace jsmapper
{
	class Device::Private
	{
	public:
		/// Device ID (minor number)
		int id;
		/// File descriptor, when opened
		int fd;
		/// Open count
		int nOpen;
		/// Device map:
		DeviceMap * map;
		
	public:
		Private()
			: id( -1 ), 
			fd( -1 ), 
			nOpen( 0 ), 
			map( NULL )
		{
		}
	};
	
	
	Device::Device( int id /*= 0*/ )
	{
		d = new Private();
		d->id = id;
	}
	
	Device::~Device()
	{
		while( isOpen() )
		{
			JSMAPPER_LOG_WARNING( "Forcing device close!" );
			close();
		}
		
		if( d->map != NULL )
		{
			delete d->map;
			d->map = NULL;
		}
		
		delete d;
		d = NULL;
	}
	
	
	std::string Device::getPath() const
	{
		return Device::getPath( d->id );
	}


    std::string /*static*/ Device::getPath( int id )
    {
        char dev[128];
        sprintf( dev, "/dev/input/jsmap%i", id );
        return std::string( dev );
    }

	//

	std::string /*static*/ Device::PREFIX			= "jsmap";
	int			/*static*/ Device::PREFIX_LENGTH	= 5;

	int /*static*/ Device::getId( const std::string &path )
	{
		int id = -1;

		// get last part of path:
		std::string name = path;
		int pos = name.find_last_of( '/' );
		if( pos != std::string::npos )
		{
			name = name.substr( pos + 1 );
		}
		if( name.length() > PREFIX_LENGTH
				&& name.substr( 0, PREFIX_LENGTH ).compare( PREFIX ) == 0 )
		{
			id = atoi( name.substr( PREFIX_LENGTH ).c_str() );
		}

		return id;
	}

	//

	bool /*static*/ Device::test( int id )
	{
		struct stat st;

		std::string path = Device::getPath( id );
		int ret = stat( path.c_str(), &st );
		return ( ret == 0 && S_ISCHR(st.st_mode) );
	}


	bool Device::open()
	{
		bool ret = false;
		
		if( d->fd < 0 )
		{
			// not opened yet - open it now
            std::string path = getPath();
			d->fd = ::open( path.c_str(), O_RDWR );
			if( d->fd >= 0 )
			{
				JSMAPPER_LOG_INFO( "Opened device '%s'", path.c_str() );
				d->nOpen = 1;
				ret = true;
			}
			else
				JSMAPPER_LOG_ERROR( "Failed to open device '%s' (error %i: %s)", path.c_str(), errno, strerror( errno ) );
		}
		else
		{
			d->nOpen++;
			ret = true;
		}
		
		return ret;
	}
		
	bool Device::isOpen() const
	{
		return (d->fd >= 0 && d->nOpen > 0 );
	}
	
	void Device::close()
	{
		if( d->nOpen > 0 )
		{
			if( d->nOpen == 1 && d->fd >= 0 )
			{
				::close( d->fd );
				d->fd = -1;
			}
			d->nOpen--;
		}
	}
	
	
	//
	// Device querying:
	//
	
	long Device::getVersion()
	{
		long result = 0;
		
		if( open() ) 
		{
			__u32 value = 0;
			int ret = ioctl( d->fd, JMIOCGVERSION, &value );
			if( ret == 0 )
			{
				result = value;
			}
			else
				JSMAPPER_LOG_ERROR( "Failed to query API version (error %i: %s)", errno, strerror( errno ) );
		
			close();
		}
		
		return result;
	}

	string Device::getName()
	{
		string result;
		
		if( open() ) 
		{
			char buf[128] = "";
			int ret = ioctl( d->fd, JMIOCGNAME(sizeof(buf)), buf );
			if( ret >= 0 )
			{
				result = buf;
			}
			else
				JSMAPPER_LOG_ERROR( "Failed to query device name (error %i: %s)", errno, strerror( errno ) );
		
			close();
		}
		
		return result;
	}

	int Device::getNumButtons()
	{
		int result = -1;
		
		if( open() ) 
		{
			__u8 value;
			int ret = ioctl( d->fd, JMIOCGBUTTONS, &value );
			if( ret == 0 )
			{
				result = (int) value;
			}
			else
				JSMAPPER_LOG_ERROR( "Failed to query number of buttons (error %i: %s)", errno, strerror( errno ) );
		
			close();
		}
		
		return result;
	}

    int Device::getNumAxes()
	{
        int result = -1;
		
		if( open() ) 
		{
			__u8 value;
			int ret = ioctl( d->fd, JMIOCGAXES, &value );
			if( ret == 0 )
			{
				result = (int) value;
			}
			else
				JSMAPPER_LOG_ERROR( "Failed to query number of axes (error %i: %s)", errno, strerror( errno ) );
		
			close();
		}
		
		return result;
	}
	
    int Device::getButtonValue( ButtonID id )
    {
        long result = 0;
		
		if( open() ) 
		{
			__s32 value = id;
			int ret = ioctl( d->fd, JMIOCGBUTTONVALUE, &value );
			if( ret == 0 )
			{
				result = value;
			}
			else
				JSMAPPER_LOG_ERROR( "Failed to query button value (error %i: %s)", errno, strerror( errno ) );
		
			close();
		}
		
		return result;
    }
    
    int Device::getAxisValue( AxisID id )
    {
        long result = 0;
		
		if( open() ) 
		{
			__s32 value = id;
			int ret = ioctl( d->fd, JMIOCGAXISVALUE, &value );
			if( ret == 0 )
			{
				result = value;
			}
			else
				JSMAPPER_LOG_ERROR( "Failed to query axis value (error %i: %s)", errno, strerror( errno ) );
		
			close();
		}
		
		return result;
    }

	
	//
	// programming functions:
	//
	
	bool Device::clear()
	{
		bool result = false;
		
		if( open () )
		{
			JSMAPPER_LOG_DEBUG( "Clearing device..." );
			int ret = ioctl( d->fd, JMIOCCLEAR );
			if( ret == 0 )
			{
				result = true;
			}
			else
				JSMAPPER_LOG_ERROR( "Failed to clear device (error %i: %s)", errno, strerror( errno ) );
			
			close();
		}
        
        return result;
	}

	std::string Device::getProfileName()
	{
		string result;

		if( open() )
		{
			char buf[1024] = "";
			int ret = ioctl( d->fd, JMIOCGPROFILENAME(sizeof(buf)), buf );
			if( ret >= 0 )
			{
				result = buf;
			}
			else
				JSMAPPER_LOG_ERROR( "Failed to query profile name (error %i: %s)", errno, strerror( errno ) );

			close();
		}

		return result;
	}

	bool Device::setProfileName( const std::string &name )
	{
		bool result = false;

		if( open() )
		{
			int ret = ioctl( d->fd, JMIOCSPROFILENAME( name.length() ), name.c_str() );
			if( ret == 0 )
			{
				result = true;
			}
			else
				JSMAPPER_LOG_ERROR( "Failed to set profile name (error %i: %s)", errno, strerror( errno ) );

			close();
		}

		return result;
	}

    
    bool Device::setButtonAction( uint modeId, ButtonID btnId, Action * action )
    {
        bool result = false;
		
		if( open () )
		{
            size_t cbBuffer = 0;
			struct t_JSMAPPER_ACTION * buffer = action->toDeviceAction( cbBuffer );
            if( buffer )
            {
                JSMAPPER_LOG_DEBUG( "Loading action for button ID=%u...", (uint) btnId );
                
				buffer->button.id    = btnId;
                buffer->mode_id      = modeId;
                int err = ioctl( d->fd, JMIOCSBUTTONACTION( cbBuffer ), buffer );
                if( err == 0 )
                {
                    result = true;
                }
                else
                    JSMAPPER_LOG_ERROR( "Failed to load action for button ID=%u into device (error %i: %s)", (uint) btnId, errno, strerror( errno ) );
                
                free( buffer );
            }
            else
                JSMAPPER_LOG_ERROR( "Failed to allocate buffer for action!" );
            
			close();
		}
        
        return result;
    }
    
    
	bool Device::setAxisAction( uint modeId, AxisID axisId, const Band &band, Action * action )
	{
		bool result = false;

		if( open () )
		{
			size_t cbBuffer = 0;
			struct t_JSMAPPER_ACTION * buffer = action->toDeviceAction( cbBuffer );
			if( buffer )
			{
				JSMAPPER_LOG_DEBUG( "Loading action for axis ID=%u, band={%i, %i}...", (uint) axisId, band.m_low, band.m_high );

				buffer->axis.id		= axisId;
				buffer->axis.low	= band.m_low;
				buffer->axis.high	= band.m_high;
				buffer->mode_id		= modeId;
                int err = ioctl( d->fd, JMIOCSAXISACTION( cbBuffer ), buffer );
				if( err == 0 )
				{
					result = true;
				}
				else
					JSMAPPER_LOG_ERROR( "Failed to load action for button ID=%u into device (error %i: %s)", (uint) axisId, errno, strerror( errno ) );

				free( buffer );
			}
			else
				JSMAPPER_LOG_ERROR( "Failed to allocate buffer for action!" );

			close();
		}

		return result;
	}


	uint Device::addMode( Condition * condition, uint parentModeId )
    {
        uint result = 0;
		
		if( open () )
		{
            struct t_JSMAPPER_MODE mode_p = { 0 };
            
			bool ok = true;
            mode_p.parent_mode_id = parentModeId;
            if( condition )
            {
                if( condition->toDeviceCondition( this, &mode_p ) == false )
					ok = false;
			}
            
			if( ok )
			{
				int err = ioctl( d->fd, JMIOCADDMODE, &mode_p );
				if( err == 0 )
				{
					JSMAPPER_LOG_DEBUG( "Created new device mode with ID=%u", mode_p.mode_id );
					result = mode_p.mode_id;
				}
				else
					JSMAPPER_LOG_ERROR( "Failed to create new child mode for ID=%u (error %i: %s)", parentModeId, errno, strerror( errno ) );
			}
            
			close();
		}
        
        return result;
    }
	
	
	//
	// Device map:
	//
	
	void Device::setDeviceMap( DeviceMap * map )
	{
		if( (d->map != NULL) && (d->map != map) )
		{
			delete d->map;
			d->map = NULL;
		}
		
		d->map = map;
	}
	
	DeviceMap * Device::getDeviceMap() const
	{
		return d->map;
	}
	
}
	
