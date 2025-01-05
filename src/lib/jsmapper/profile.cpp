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
 * \file profile.cpp
 * \brief Implementation file for Profile class
 * \author Eduard Huguet <eduardhc@gmail.com>
 */

#include "profile.h"
#include "mode.h"
#include "log.h"
#include "device.h"
#include "action.h"
#include "xmlhelpers.h"

#include <string.h>
#include <libxml/encoding.h>

#include <map>


// using namespace std;

namespace jsmapper
{
	/**
	 * \brief Profile's private internal class
	 */
	class Profile::Private
	{
	public:
		/// Profile target device name
		std::string target;
		/// Profile name, for user reference
		std::string name;
		/// Profile description, for user reference
		std::string description;
        /// Available actions, mapped through their name
        std::map<std::string, Action *> actions;
		/// Profile's root mode
		Mode * rootMode;
		
	public:
		Private()
			: rootMode( NULL )
		{
		}
	};
	
	
    Profile::Profile( const std::string &target /*= std::string()*/ )
	{
		d = new Private();
		d->target = target;
		
		// create root mode
		d->rootMode = new Mode( this );
	}
	
	Profile::~Profile()
	{
        clear();

		if( d->rootMode )
		{
			delete d->rootMode;
			d->rootMode = NULL;
		}
		
		delete d;
		d = NULL;
	}

    void Profile::setTarget( const std::string &target )
    {
        d->target = target;
    }
    
	const std::string & Profile::getTarget() const
	{
		return d->target;
	}
	

    //
	
	void Profile::clear()
	{
		d->name.clear();
		d->description.clear();

		if( d->rootMode )
		{
			delete d->rootMode;
			d->rootMode = NULL;
		}
		
        std::map<std::string, Action *>::iterator it = d->actions.begin();
        while( it != d->actions.end() )
        {
            delete (*it++).second;
        }
        d->actions.clear();

		d->rootMode = new Mode( this );
	}
	
	
	void Profile::setName( const std::string &name )
	{
		d->name = name;
	}
	
	const std::string & Profile::getName() const
	{
		return d->name;
	}
	
	void Profile::setDescription( const std::string &desc )
	{
		d->description = desc;
	}
	
	const std::string & Profile::getDescription() const
	{
		return d->description;
	}
	
	
    //

    void Profile::addAction( Action * action )
    {
        std::map<std::string, Action *>::iterator it = d->actions.find( action->getName() );
        if( it != d->actions.end() )
        {
            JSMAPPER_LOG_WARNING( "Overriding previous action named '%s'", action->getName().c_str() );
            delete (*it).second;
        }

        d->actions[ action->getName() ] = action;
    }

	
	std::list<std::string> Profile::getActionNames() const
	{
		std::list<std::string> result;
		
		std::map<std::string, Action *>::const_iterator it = d->actions.begin();
        while( it != d->actions.end() )
        {
			result.insert( result.end(), (*it++).first );
        }
		
		return result;
	}
	

    Action * Profile::getAction( const std::string &name ) const
    {
        Action * action = NULL;

        std::map<std::string, Action *>::const_iterator it = d->actions.find( name );
        if( it != d->actions.end() )
        {
            action = (*it).second;
        }
        else
            JSMAPPER_LOG_WARNING( "Action named '%s' not found", name.c_str() );

        return action;
    }


    void Profile::removeAction( const std::string &name )
    {
        std::map<std::string, Action *>::iterator it = d->actions.find( name );
        if( it != d->actions.end() )
        {
            delete (*it).second;
            d->actions.erase( it );
        }
        else
            JSMAPPER_LOG_WARNING( "Action named '%s' not found", name.c_str() );
    }


    //


    Mode * Profile::getRootMode() const
    {
        return d->rootMode;
    }

    void Profile::setRootMode( Mode * rootMode )
    {
        if( d->rootMode && d->rootMode != rootMode )
            delete d->rootMode;

        d->rootMode = rootMode;
    }

    //
	// XML serialization
	//
	
	bool Profile::load( const std::string &file )
	{
		bool ret = false;
		
		xmlDocPtr doc = xmlReadFile( file.c_str(), NULL, 0 );
		if( doc )
		{
			xmlNodePtr node = xmlDocGetRootElement( doc );
			if( node && strcmp( (const char *) node->name, JSMAPPER_XML_TAG_PROFILE ) == 0 )
			{
				ret = fromXml( node );
			}
			else
				JSMAPPER_LOG_ERROR( "Invalid root element on document file '%s'", file.c_str() );
			
			xmlFreeDoc( doc );
		}
		else
			JSMAPPER_LOG_ERROR( "Unable to load file '%s'", file.c_str() );
		
		xmlCleanupParser();
		return ret;
	}
	
	
	bool Profile::save( const std::string &file ) const
	{
		bool ret = false;

		xmlNodePtr node = toXml();
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
			
		return ret;
	}
	
	
	xmlNodePtr /*virtual*/ Profile::toXml() const
	{
		xmlNodePtr node = xmlNewNode( NULL, BAD_CAST JSMAPPER_XML_TAG_PROFILE );
		if( node )
		{
			// add target, name & description:
            if( d->name.empty() == false )
                xmlNewProp( node, BAD_CAST JSMAPPER_XML_TAG_NAME, BAD_CAST d->name.c_str() );
            if( d->target.empty() == false )
                xmlNewProp( node, BAD_CAST JSMAPPER_XML_TAG_TARGET, BAD_CAST d->target.c_str() );
			if( d->description.empty() == false )
			{
				xmlNodePtr nodeDesc = xmlNewTextElem( BAD_CAST JSMAPPER_XML_TAG_DESCRIPTION, d->description );
				if( nodeDesc )
				{
					xmlAddChild( node, nodeDesc );
				}
			}

			// save actions list:
			xmlNodePtr nodeActions = actionsToXml();
			if( nodeActions)
				xmlAddChild( node, nodeActions );

			// add root mode node:
			if( d->rootMode )
			{
				xmlNodePtr nodeMode = d->rootMode->toXml();
				if( nodeMode )
				{
					xmlAddChild( node, nodeMode );
				}
			}
		}
		 
		return node;
	}
	
	bool /*virtual*/ Profile::fromXml( xmlNodePtr node )
	{
		bool ret = true;
		
		clear();
		
		d->name = xmlGetStringProp( node, BAD_CAST JSMAPPER_XML_TAG_NAME );
        d->target = xmlGetStringProp( node, BAD_CAST JSMAPPER_XML_TAG_TARGET );
		
		xmlNodePtr subNode = node->children;
		while( subNode && ret )
		{
			if( subNode->type == XML_ELEMENT_NODE) 
			{
                std::string tag = (const char *) subNode->name;
				if( tag == JSMAPPER_XML_TAG_DESCRIPTION )
				{
					// read description:
					d->description = xmlGetTextElem( subNode );
				}
                else if( tag == JSMAPPER_XML_TAG_ACTIONS )
                {
					// load action list:
					actionsFromXml( subNode );
                }
                else if( tag == JSMAPPER_XML_TAG_MODE )
				{
					// read root mode:
					Mode * rootMode = new Mode( this );
					if( rootMode->fromXml( subNode ) )
					{
						setRootMode( rootMode );
					}
					else
					{
						JSMAPPER_LOG_ERROR( "Failed to load root mode!" );
						
						delete rootMode;
						rootMode = NULL;
					}
				}
			}
			
			subNode = subNode->next;
		}
		
		return ret;
	}
    
    
	//

	xmlNodePtr Profile::actionsToXml() const
	{
		xmlNodePtr node = xmlNewNode( NULL, BAD_CAST JSMAPPER_XML_TAG_ACTIONS );

		std::map<std::string, Action *>::const_iterator it = d->actions.begin();
		while( it != d->actions.end() )
		{
			Action * action = (*it++).second;

			xmlNodePtr nodeAction = action->toXml();
			if( nodeAction )
			{
				xmlAddChild( node, nodeAction );
			}
		}

		return node;
	}

	bool Profile::actionsFromXml( xmlNodePtr node )
	{
		bool ret = true;

		xmlNodePtr subNode = node->children;
		while( subNode && ret )
		{
			if( subNode->type == XML_ELEMENT_NODE)
			{
				std::string tag = (const char *) subNode->name;
				if( tag == JSMAPPER_XML_TAG_ACTION )
				{
					Action * action = Action::buildFromXml( subNode );
					if( action )
					{
						addAction( action );
					}
				}
			}

			subNode = subNode->next;
		}

		return ret;
	}


    //
    // Device interaction
    //
    
    bool Profile::toDevice( Device * dev )
    {
        bool ret = false;
        
        // clear device first:
        if( dev->open() )
        {
            // clear device first:
            JSMAPPER_LOG_DEBUG( "Clearing device..." );
            dev->clear();
            
            // enter load mode:
            Mode * mode = getRootMode();
            if( mode )
            {
                JSMAPPER_LOG_DEBUG( "Loading root mode into device..." );
                ret = mode->toDevice( dev );
            }
            else
                JSMAPPER_LOG_WARNING( "No root mode to load!" );
            
			// finally, if succesful set loaded profile name:
			if( ret )
			{
				ret = dev->setProfileName( d->name );
			}

            dev->close();
        }
        
        return ret;
    }
}
