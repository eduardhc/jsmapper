/**
 * Copyright 2013 Eduard Huguet Cuadrench
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2.1 of the License.
 * 
 * Foobar is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * \file main.cpp
 * \brief JSMapper userspace device information tool
 * \author Eduard Huguet <eduardhc@gmail.com>
 */

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <errno.h>
#include <unistd.h>

#include <ncurses.h>

#include <jsmapper/device.h>
#include <jsmapper/devicemap.h>
#include <jsmapper/log.h>


/// Short options list:
static const char	 shortOptions[] = "hd:cv";

/// Long options list:
static struct option longOptions[]	=
{
	{"help",    no_argument,        NULL, 'h'},
	{"device",	required_argument,  NULL, 'd'},
    {"create",	no_argument,        NULL, 'c'},
    {"view",	no_argument,        NULL, 'v'},
	{ 0, 0, 0, 0 }
};

static const char * helpText =		
	"Usage:\n"
	"    jsmapper-device [options] file\n"
	"\n"
	"Options:\n"
	"    -d,--device <dev>      use specific jsmap device id (default is 0, for jsmap0)\n"
	"    -c,--create            creates initial device map file\n"
	"    -v,--view              displays joystick device values using map file\n"
	"    -h,--help              shows this help\n"
	"\n";

static const char * noOpText =		
    "Please, specify either \"create\" or \"view\" options!"
    "\n";

static const char * noFileText =		
    "Please, specify a device map file!"
    "\n";


/**
    \brief Device map creation function
 */
static int createMap( int deviceId, const std::string &mapFile );

/**
    \brief Device map viewing function
 */
static int viewMap( int deviceId, const std::string &mapFile );


/**
  \brief Entry point
  */
int main(int argc, char **argv) 
{
    // process options:
	int showHelp = 1;
	int deviceId = 0;
	std::string mapFile;
	int create = 0;
	int view = 0;
	        
	int error = 0;
	char option = -1;
	int optionIndex = 0;
	while( (option = getopt_long( argc, argv, shortOptions, longOptions, &optionIndex )) != -1 && error == 0 )
	{
		if(option == 0 )
			option = longOptions[ optionIndex ].val;
		
		switch( option )
		{
		case 'h':
			showHelp = 1;
			break;
		
		case 'd':
			if( optarg )
			{
				deviceId = atoi( optarg );
				showHelp = 0;
			}
			break;
		
		case 'c':
			create = 1;
			showHelp = 0;
			break;
		
        case 'v':
            view = 1;
            showHelp = 0;
            break;
            
		case '?':
			error = 1;
			showHelp = 1;
			break;
			
		default:
			break;
		}
	}
    if( optind < argc )
    {
        // assume final parameter is map file:
        mapFile = argv[ optind ];
    }

	
	// show help if asked to (or no params were given)
	if( showHelp || error )
	{
        puts( helpText );
		return EINVAL;		
	}

    
    // check any op is specified:
    if( create == 0 && view == 0 )
    {
        puts( noOpText );
        return EINVAL;		
    }

    // go ahead:
    int result = 0;
    if( create )    
    {
        if( mapFile.empty() )
        {
            fputs( noFileText, stderr );
            return EINVAL;		
        }
        
        result = createMap( deviceId, mapFile );
    }
    
    if( view && result == 0 )
    {
        result = viewMap( deviceId, mapFile );
    }
        
    return result;
}



//
// Implementation
//

int createMap( int deviceId, const std::string &file )
{
    int result = EINVAL;
    
    jsmapper::Device dev( deviceId );
    if( dev.open() )
    {
		jsmapper::DeviceMap map;
        if( map.init( &dev ) )
        {
            if( map.save( file ))
            {
                printf( "Created new device map file %s\n", file.c_str() );
                result = 0;
            }
            else
                fputs( "Failed to save map file!", stderr );
        }
        else
            fputs( "Failed to intialize map for device!\n", stderr );
        
        dev.close();
    }
    
    return result;
}


//

static const char * deviceText		= "Device path     : %s\n";
static const char * nameText		= "Device name     : %s\n";
static const char * mapFileText		= "Device map file : %s\n";
static const char * buttonsText		= "\nButtons:\n";
static const char * axesText		= "\nAxes:\n";
static const char * stopMsg			= "\nPress Ctrl+C to stop...\n\n";

int viewMap( int deviceId, const std::string &file )
{
    jsmapper::Log::getLog()->setLogLevel( jsmapper::Log::NONE );		// disable logging - interferes witg ncurses...
    initscr();
    
    // loop until Ctrl+C is pressed:
    while( true )
    {
		jsmapper::Device dev( deviceId );
		
		erase();
        move( 0, 0 );
		printw( deviceText, dev.getPath().c_str() );
		
        if( dev.open() )
        {
			printw( nameText, dev.getName().c_str() );
			
            std::string real_file = file;
            if( real_file.empty() )
                real_file = jsmapper::DeviceMap::find( &dev );
            
            if( real_file.empty() == false )
            {
				jsmapper::DeviceMap map( &dev );
                if( map.load( real_file ) ) 
                {
                    printw( mapFileText, real_file.c_str() );
                    
                    printw( buttonsText );
                    int numButtons = dev.getNumButtons();
                    for( jsmapper::ButtonID id = 0; id < numButtons; id++ )
                    {
                        std::string name = map.getButtonName( id );
                        int value = dev.getButtonValue( id );
                        printw( "%2i [%-12s]: %i\n", (int) id, name.c_str(), value );
                    }
                    
                    printw( axesText );
                    int numAxes = dev.getNumAxes();
                    for( jsmapper::AxisID id = 0; id < numAxes; id++ )
                    {
                        std::string name = map.getAxisName( id );
                        int value = dev.getAxisValue( id );
                        printw( "%2i [%-12s]: %i\n", (int) id, name.c_str(), value );
                    }
                }
                else
                {
                    printw( mapFileText, "<Failed to load map file!>" );
                }
            }
            else
            {
                printw( mapFileText, "<Failed to find a map file for device!>" );
            }
            
            dev.close();
        }
        else
			printw( nameText, "<Failed to open device!>" );

        printw( stopMsg );
        
        refresh();
        usleep( 500000 );
    }
    
    endwin();
    return 0;
}

