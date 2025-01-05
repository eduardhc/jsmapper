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
 * \file devicemap.cpp
 * \author Eduard Huguet <eduardhc@gmail.com>
 * \brief Implementation file for 'DeviceMap' class
 */

#include "devicemap.h"
#include "device.h"
#include "log.h"
#include "xmlhelpers.h"

#include <map>
#include <string.h>

#include <dirent.h>

using namespace std;

namespace jsmapper
{
    static const std::string DevicesFolder = JSMAPPER_INSTALL_PREFIX "/share/jsmapper/devices/";
    
	class DeviceMap::Private
	{
	public:
		/// Device name
		std::string name;
		/// Button names
		std::map<ButtonID, std::string> buttonNames;
		/// Reverse mapping for button names
		std::map<std::string, ButtonID> buttonIDs;
		/// Axis names
		std::map<AxisID, std::string> axisNames;
		/// Reverse mapping for axis names
		std::map<std::string, AxisID> axisIDs;
		/// File path
		std::string path;
		

	public:
		Private()
		{
		}
	};
	
	
	//
	
	DeviceMap::DeviceMap( const std::string & name /*= std::string()*/ )
	{
		d = new Private();		
		d->name = name;
	}

	DeviceMap::DeviceMap( Device * dev )
	{
		d = new Private();		
		init( dev );
	}
	
	DeviceMap::~DeviceMap() 
	{
		delete d;
		d = NULL;
	}

	//	
	
	const std::string& DeviceMap::getName() const
	{
		return d->name;
	}
	
	const std::string& DeviceMap::getPath() const
	{
		return d->path;
	}
	
	//	
	
    bool DeviceMap::init( Device * dev )
    {
        bool ret = false;
        
        clear();
        if( dev->open() )
        {
			// retrieve device name:
			d->name = dev->getName();
			
            // retrieve list of buttons:
            int numButtons = dev->getNumButtons();
            for( ButtonID id = 0; id < numButtons; id++ )
            {
                std::string name = figureButtonName( id );
                if( name.empty() == false )
                {
                    setButtonName( id, name );
                }
                else
                    JSMAPPER_LOG_WARNING( "Unable to figure a button name for ID=%u", (uint) id );
            }
            
            // retrieve list of axes:
            int numAxes = dev->getNumAxes();
            for( AxisID id = 0; id < numAxes ; id++ )
            {
                std::string name = figureAxisName( id );
                if( name.empty() == false )
                {
                    setAxisName( id, name );
                }
                else
                    JSMAPPER_LOG_WARNING( "Unable to figure a button name for ID=%u", (uint) id );
            }
            
            dev->close();
            ret = true;
        }
        
        return ret;
    }

	
    void DeviceMap::clear()
    {
		d->name = std::string();
		d->path = std::string();
        d->buttonIDs.clear();
        d->buttonNames.clear();
        d->axisIDs.clear();
        d->axisNames.clear();
    }


	//
	// Mapping functions:
	//
	
	ButtonID DeviceMap::getButtonID( const std::string &name ) const
	{
		ButtonID result = INVALID_BUTTON_ID;
		
		std::map<std::string, ButtonID>::const_iterator it = d->buttonIDs.find( name );
		if( it != d->buttonIDs.end() )
		{
			result = (*it).second;
		}
		
		return result;
	}

    
	std::string DeviceMap::getButtonName( ButtonID id ) const
	{
		std::string result;
		
		std::map<ButtonID, std::string>::const_iterator it = d->buttonNames.find( id );
		if( it != d->buttonNames.end() )
		{
			result = (*it).second;
		}
		
		return result;
		
	}
	
    
    void DeviceMap::setButtonName( ButtonID id, const std::string &name )
    {
        if( name.empty() == false )
        {
            std::map<std::string, ButtonID>::iterator it = d->buttonIDs.find( name );
            if( it != d->buttonIDs.end() )
            {
                d->buttonIDs.erase( it );
            }
        } 
        
        if( id != INVALID_BUTTON_ID )
        {
            std::map<ButtonID, std::string>::iterator it = d->buttonNames.find( id );
            if( it != d->buttonNames.end() )
            {
                d->buttonNames.erase( it );
            }
        }
        
        if( name.empty() == false  && id != INVALID_BUTTON_ID )
        {
            d->buttonIDs[ name ] = id;
            d->buttonNames[ id ] = name;
        }
    }
    

	AxisID DeviceMap::getAxisID( const std::string &name ) const
	{
		AxisID result = INVALID_AXIS_ID;
		
		std::map<std::string, AxisID>::const_iterator it = d->axisIDs.find( name );
		if( it != d->axisIDs.end() )
		{
			result = (*it).second;
		}
		
		return result;
	}
	
	std::string DeviceMap::getAxisName( AxisID id ) const
	{
		std::string result;
		
		std::map<AxisID, std::string>::const_iterator it = d->axisNames.find( id );
		if( it != d->axisNames.end() )
		{
			result = (*it).second;
		}
		
		return result;
	}
	
    void DeviceMap::setAxisName( AxisID id, const std::string &name )
    {
        if( name.empty() == false )
        {
            std::map<std::string, AxisID>::iterator it = d->axisIDs.find( name );
            if( it != d->axisIDs.end() )
            {
                d->axisIDs.erase( it );
            }
        } 
        
        if( id != INVALID_AXIS_ID )
        {
            std::map<AxisID, std::string>::iterator it = d->axisNames.find( id );
            if( it != d->axisNames.end() )
            {
                d->axisNames.erase( it );
            }
        }
        
        if( name.empty() == false  && id != INVALID_AXIS_ID )
        {
            d->axisIDs[ name ] = id;
            d->axisNames[ id ] = name;
        }
    }


    //
    
    std::string DeviceMap::figureButtonName( ButtonID id )
    {
        char buffer[ 20 ];
        snprintf( buffer, sizeof( buffer ), "%u", (uint) id );
        return buffer;
    }

    std::string DeviceMap::figureAxisName( AxisID id )
    {
        char buffer[ 20 ];
        snprintf( buffer, sizeof( buffer ), "%u", (uint) id );
        return buffer;
    }
    

	//
	// Device map load / save
	//
	
    bool DeviceMap::load( const std::string &file )
	{
		bool ret = false;
		
		clear();

		xmlDocPtr doc = xmlReadFile( file.c_str(), NULL, 0 );
		if( doc )
		{
			xmlNodePtr node = xmlDocGetRootElement( doc );
			if( node && strcmp( (const char *) node->name, JSMAPPER_XML_TAG_DEVICE ) == 0 )
			{
				ret = mapFromXml( node );
			}
			else
				JSMAPPER_LOG_ERROR( "Invalid root element on file '%s'", file.c_str() );
			
			xmlFreeDoc( doc );
		}
		else
			JSMAPPER_LOG_ERROR( "Unable to load file '%s'", file.c_str() );
        
        xmlCleanupParser();
		
		d->path = file;
		return ret;
	}
	

    bool DeviceMap::save( const std::string &file )
    {
        bool ret = false;

		xmlNodePtr node = mapToXml();
		if( node )
		{
			xmlDocPtr doc = xmlNewDoc( BAD_CAST "1.0" );
			if( doc )
			{
				xmlDocSetRootElement( doc, node );
				
				int err = xmlSaveFormatFile( file.c_str(), doc, 1 );
				if( err >= 0 )
				{
					ret = true;
				}
				else
					JSMAPPER_LOG_ERROR( "Unable to save document to file '%s' (error: %i)", file.c_str(), err );
				
				xmlFreeDoc( doc );
			}
		}
		
		d->path = file;
		return ret;
    }
	
    
    //
		
	bool /*virtual*/ DeviceMap::mapFromXml( xmlNodePtr node )
	{
		bool ret = true;
		
		d->name = xmlGetStringProp( node, BAD_CAST JSMAPPER_XML_TAG_NAME );
		
		xmlNodePtr subNode = node->children;
		while( subNode && ret )
		{
			if( subNode->type == XML_ELEMENT_NODE) 
			{
				std::string tag = (const char *) subNode->name;
				if( tag == JSMAPPER_XML_TAG_BUTTON )
				{
					// read description:
					std::string name = xmlGetStringProp( subNode, BAD_CAST JSMAPPER_XML_TAG_NAME );
					ButtonID id = (ButtonID) xmlGetIntProp( subNode, BAD_CAST JSMAPPER_XML_TAG_ID );
					if( name.empty() == false && id != INVALID_BUTTON_ID )
					{
						d->buttonIDs[ name ] = id;
						d->buttonNames[ id ] = name;
					}
				}
				else if( tag == JSMAPPER_XML_TAG_AXIS )
				{
					std::string name = xmlGetStringProp( subNode, BAD_CAST JSMAPPER_XML_TAG_NAME );
					AxisID id = (AxisID) xmlGetIntProp( subNode, BAD_CAST JSMAPPER_XML_TAG_ID );
					if( name.empty() == false && id != INVALID_AXIS_ID )
					{
						d->axisIDs[ name ] = id;
						d->axisNames[ id ] = name;
					}
				}
			}
			
			subNode = subNode->next;
		}
		
		return ret;
	}
	
    
    xmlNodePtr DeviceMap::mapToXml()
    {
        xmlNodePtr rootNode = xmlNewNode( NULL, BAD_CAST JSMAPPER_XML_TAG_DEVICE );
		if( rootNode )
		{
            // save device name:
            xmlNewProp( rootNode, BAD_CAST JSMAPPER_XML_TAG_NAME, BAD_CAST d->name.c_str() );

            // save button names:
            std::map<ButtonID, std::string>::const_iterator itBtn = d->buttonNames.begin();
            while( itBtn != d->buttonNames.end() )
            {
                ButtonID id = (*itBtn).first;
                std::string name = (*itBtn++).second;
                
                xmlNodePtr btnNode = xmlNewNode( NULL, BAD_CAST JSMAPPER_XML_TAG_BUTTON );
                if( btnNode )
                {
                    xmlNewIntProp( btnNode, BAD_CAST JSMAPPER_XML_TAG_ID, id );
                    xmlNewProp( btnNode, BAD_CAST JSMAPPER_XML_TAG_NAME, BAD_CAST name.c_str() );
                    
                    xmlAddChild( rootNode, btnNode );
                }
            }

            // save axes names:
            std::map<AxisID, std::string>::const_iterator itAxis = d->axisNames.begin();
            while( itAxis != d->axisNames.end() )
            {
                AxisID id = (*itAxis).first;
                std::string name = (*itAxis++).second;
                
                xmlNodePtr axisNode = xmlNewNode( NULL, BAD_CAST JSMAPPER_XML_TAG_AXIS );
                if( axisNode )
                {
                    xmlNewIntProp( axisNode, BAD_CAST JSMAPPER_XML_TAG_ID, id );
                    xmlNewProp( axisNode, BAD_CAST JSMAPPER_XML_TAG_NAME, BAD_CAST name.c_str() );
                    
                    xmlAddChild( rootNode, axisNode );
                }
            }
        }
        
        return rootNode;
    }


	//
	// Device map searching
	//	
	
	void /*static*/ DeviceMap::enumerate( ENUMDEVICEMAPSPROC fn, void * data )
	{
		// open devices folder:
        DIR * dir = opendir( DevicesFolder.c_str() );
        if( dir )
        {
			bool stop = false;
            struct dirent * entry = NULL;
            while( (entry = readdir( dir )) != NULL && (stop == false) )
            {
                if( entry->d_name[0] == '.' )
                    continue;   // skip '.' & '..'
                
				std::string file = DevicesFolder + std::string( entry->d_name );
				
				// try to load map, then call callback:
				DeviceMap map;
				if( map.load( file ) ) 
				{
					if( fn( &map, data ) == false ) 
					{
						stop = true;
					}
				}
			}
				
            closedir( dir );
        }
	}

	std::string /*static*/ DeviceMap::find( Device * dev )
    {
        std::string result;
        
        std::string name = dev->getName();
        if( name.empty() == false )
        {
            result = DeviceMap::find( name );
        }
        
        return result;
    }


	struct t_FINDDATA 
	{
		std::string name;
		std::string result;
	};

	static bool _find( DeviceMap * map, void * data )
	{
		bool ret = true;
		
		struct t_FINDDATA * findData = (struct t_FINDDATA *) data;
		if( strcmp( findData->name.c_str(), map->getName().c_str() ) == 0 )
		{
			JSMAPPER_LOG_DEBUG( "Found device map file '%s' for '%s'", map->getPath().c_str(), findData->name.c_str() );
			findData->result = map->getPath();
			ret = false;	// stop enumeration
		}
		
		return ret;
	}
	
    std::string /*static*/ DeviceMap::find( const std::string &name )
    {
		struct t_FINDDATA data;
		data.name = name;
		enumerate( &_find, &data );
		return data.result;
    }

    
}
