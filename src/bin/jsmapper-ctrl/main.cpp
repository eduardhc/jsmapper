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
 * \brief JSMapper userspace control tool
 * \author Eduard Huguet <eduardhc@gmail.com>
 */

#include <stdio.h>
#include <string>
#include <getopt.h>

#include <jsmapper/device.h>
#include <jsmapper/devicemap.h>
#include <jsmapper/profile.h>
#include <jsmapper/keymap.h>
#include <jsmapper/log.h>

#include <linux/drivers/input/jsmapper_api.h>
#include <linux/input.h>

#define SHOW_AXES       1000
#define SHOW_BUTTONS    1001
#define SHOW_KEYS       1002


/// Short options list:
static const char	 shortOptions[] = "hd:cl:m:";

/// Long options list:
static struct option longOptions[]	=
{
	{"help",    no_argument,        NULL, 'h'},
	{"device",	required_argument,  NULL, 'd'},
	{"clear",   no_argument,        NULL, 'c'},
	{"load",	required_argument,  NULL, 'l'},
    {"map",		required_argument,  NULL, 'm'},
    {"keys",    no_argument,        NULL, SHOW_KEYS },
    {"axes",    no_argument,        NULL, SHOW_AXES },
    {"buttons", no_argument,        NULL, SHOW_BUTTONS },
	{ 0, 0, 0, 0 }
};

static const char * helpText =		
	"Usage:\n"
	"    jsmapper-ctrl [options]\n"
	"\n"
	"Options:\n"
	"    -d,--device <dev>      use specific jsmap device id (default is 0, for jsmap0)\n"
	"    -c,--clear             clear device\n"
	"    -l,--load <file>       load specified profile file\n"
	"    -m,--map <file>        uses specific device map file\n"
	"    -h,--help              shows this help\n"
	"\n"
	"Dump options:\n"
	"    --keys                 dumps supported key list\n"
    "    --buttons              dumps supported buttons list\n"
    "    --axes                 dumps supported axes list\n"
	"\n";


void printKeys();
void printAxes();
void printButtons();
bool initMap( jsmapper::Device &dev, std::string &mapFile );


/**
  \brief CLI entry point
  */

int main(int argc, char **argv) 
{
	// process options:
	int showHelp = 1;
    int showAxes = 0;
    int showButtons = 0;
    int showKeys = 0;
	int deviceId = 0;
	std::string profileFile;
	std::string mapFile;
	int clearFilter	= 0;
	        
	int error = 0;
	int option = -1;
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
			clearFilter = 1;
			showHelp = 0;
			break;
		
		case 'l':
			if( optarg )
			{
				profileFile = optarg;
				showHelp = 0;
			}
			break;
		
		case 'm':
			if( optarg )
			{
				mapFile = optarg;
				showHelp = 0;
			}
			break;
		
        case SHOW_AXES:
            showAxes = 1;
            showHelp = 0;
            break;
            
        case SHOW_BUTTONS:
            showButtons = 1;
            showHelp = 0;
            break;
            
        case SHOW_KEYS:
            showKeys = 1;
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
	if( error )
		return error;

	
	// show help if asked to (or no params were given)
	if( showHelp )
	{
        puts( helpText );
		return 1;		
	}

	
	jsmapper::Log::getLog()->setLogLevel( jsmapper::Log::DEBG );		// TODO set up a setting for log level
	
	// do what asked for:
	if( profileFile.empty() == false 
			|| clearFilter )
	{
		jsmapper::Device dev( deviceId );
		if( dev.open() )
		{
			// clear filter, if asked to:
			if( clearFilter && error == 0 )
			{
				printf( "Clearing device...\n" );
				if( dev.clear() == false )
				{
					fprintf( stderr, "Failed to clear device!\n" );
					error = 1;
				}
			}
			
			
			// load a profile, if asked to:
			if( profileFile.empty() == false && error == 0 )
			{
				printf( "Loading profile from %s...\n", profileFile.c_str() );
				jsmapper::Profile profile;
				if( profile.load( profileFile ) )
				{
					// ok, load profile into device:
					if( initMap( dev, mapFile ) )
					{
						if( profile.toDevice( &dev ) == false ) 
						{
							fprintf( stderr, "Failed to load profile into device!\n" );
							error = 1;
						}
					}
				}
				else
				{
					fprintf( stderr, "Failed to load profile file '%s'!\n", profileFile.c_str() );
					error = 1;
				}
			}
			
			dev.close();
		}
		else
		{
			fprintf( stderr, "Failed to open device!\n" );
			error = 1;
		}
	}

    if( error == 0 )
    {
        if( showKeys )
            printKeys();
        if( showAxes )
            printAxes();
        if( showButtons )
            printButtons();
    }	
    
	return error;
}


bool initMap( jsmapper::Device &dev, std::string &mapFile ) 
{
	bool ret = false;
	
	// try to find an appropiate device map file, if none given:
	if( mapFile.empty() ) 
	{
		mapFile = jsmapper::DeviceMap::find( &dev );
	}
	
	jsmapper::DeviceMap * map = new jsmapper::DeviceMap( &dev );
	if( mapFile.empty() )
	{
		printf( "Warning: no suitable map file found for device '%s'\n", dev.getName().c_str() );
	}
	else
	{
		if( map->load( mapFile ) )
		{
			ret = true;
		}
		else
			fprintf( stderr, "Failed to load device map file: '%s'", mapFile.c_str() );
	}
	dev.setDeviceMap( map );
	
	return ret;
}


void printAxes()
{
    jsmapper::KeyMap * map = jsmapper::KeyMap::instance();

    printf( "Supported axes list:\n" );        
    std::list<std::string> syms = map->getRelAxesSymbols();
    std::list<std::string>::const_iterator it = syms.begin();
    while( it != syms.end() )
    {
        std::string sym = *it++;
        uint id = map->getRelAxisId( sym );
        printf( "  %-16s %8u (0x%04x)\n", sym.c_str(), id, id );
    }
    
    printf( "\n" );
}

void printButtons()
{
    jsmapper::KeyMap * map = jsmapper::KeyMap::instance();

    printf( "Supported buttons list:\n" );        
    std::list<std::string> syms = map->getButtonsSymbols();
    std::list<std::string>::const_iterator it = syms.begin();
    while( it != syms.end() )
    {
        std::string sym = *it++;
        uint id = map->getButtonId( sym );
        printf( "  %-16s %8u (0x%04x)\n", sym.c_str(), id, id );
    }
    
    printf( "\n" );
}

void printKeys()
{
    jsmapper::KeyMap * map = jsmapper::KeyMap::instance();

    printf( "Supported key list:\n" );        
    std::list<std::string> syms = map->getKeySymbols();
    std::list<std::string>::const_iterator it = syms.begin();
    while( it != syms.end() )
    {
        std::string sym = *it++;
        uint id = map->getKeyId( sym );
        printf( "  %-16s %8u (0x%04x)\n", sym.c_str(), id, id );
    }
    
    printf( "\n" );
}


