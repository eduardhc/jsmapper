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
 * \file macroaction.cpp
 * \brief Implementation file for key sequence simulation class
 * \author Eduard Huguet <eduardhc@gmail.com>
 */

#include "macroaction.h"
#include "xmlhelpers.h"
#include "keymap.h"
#include "log.h"

#include <string.h>

namespace jsmapper
{
	/**
	  \brief MacroAction private implementation class
	  */
	class MacroAction::Private
	{
	public:
		/// List of keystrokes to send
		KeyList keys;
		/// Keystroke spacing, in ms
		uint spacing;

	public:
		Private()
			: spacing( DefaultSpacing )
		{
		}
	};

    const int MacroAction::DefaultSpacing = 250;


	//
	// ctor / dtor:
	//

	MacroAction::MacroAction( const std::string &name /*= std::string()*/,
                                bool filter /*= true*/,
                                const std::string description /*= std::string() */)
		: Action( MacroActionType, name, filter, description )
	{
		d = new Private();
	}


	MacroAction::~MacroAction()
	{
		delete d;
		d = NULL;
	}


	//
	// attributes
	//

	void MacroAction::addKey( const Key &key )
	{
		d->keys.push_back( key );
	}

	void MacroAction::addKey( uint key, uint modifiers /*= 0*/ )
	{
		addKey( Key( key, modifiers ) );
	}

	const MacroAction::KeyList &MacroAction::getKeys() const
	{
		return d->keys;
	}

	void MacroAction::clearKeys()
	{
		d->keys.clear();
	}


	//

	void MacroAction::setSpacing( uint ms )
	{
		d->spacing = ms;
	}

	uint MacroAction::getSpacing() const
	{
		return d->spacing;
	}


	//
	// serialization
	//

	xmlNodePtr /*virtual*/ MacroAction::toXml() const
	{
		xmlNodePtr node = Action::toXml();
		if( node )
		{
			xmlNewProp( node, BAD_CAST JSMAPPER_XML_TAG_TYPE, BAD_CAST JSMAPPER_XML_TYPE_MACRO );
			xmlNewIntProp( node, BAD_CAST JSMAPPER_XML_TAG_SPACING, d->spacing );

			xmlNodePtr nodeKeys = keysToXml();
			if( nodeKeys )
			{
				xmlAddChild( node, nodeKeys );
			}
		}

		return node;
	}

	bool /*virtual*/ MacroAction::fromXml( xmlNodePtr node )
	{
		bool ret = Action::fromXml( node );
		if( ret )
		{
			d->spacing = xmlGetIntProp( node, BAD_CAST JSMAPPER_XML_TAG_SPACING, d->spacing );
			
			xmlNodePtr subNode = node->children;
			while( subNode && ret )
			{
				if( subNode->type == XML_ELEMENT_NODE) 
				{
					std::string tag = (const char *) subNode->name;
					if( tag == JSMAPPER_XML_TAG_KEYS )
					{
						keysFromXml( subNode );
					}
				}
			
				subNode = subNode->next;
			}
		}
		
		return ret;
	}


	//
	
	
	xmlNodePtr MacroAction::keysToXml() const
	{
		xmlNodePtr nodeKeys = NULL;
		
		if( d->keys.empty() == false )
		{
			nodeKeys = xmlNewNode( NULL, BAD_CAST JSMAPPER_XML_TAG_KEYS );
			
			KeyList::const_iterator it = d->keys.begin();
			while( it != d->keys.end() )
			{
				const Key &key = *it++;
	
				xmlNodePtr nodeKey = xmlNewNode( NULL, BAD_CAST JSMAPPER_XML_TAG_KEY );
					
					xmlNewProp( nodeKey, BAD_CAST JSMAPPER_XML_TAG_KEY, BAD_CAST KeyMap::instance()->getKeySymbol( key.id ).c_str() );
	
					std::list<std::string> lstModif = KeyMap::instance()->getModifiersSymbols( key.modifiers );
					xmlNewStringListProp( nodeKey, BAD_CAST JSMAPPER_XML_TAG_MODIFIERS, lstModif );
	
				xmlAddChild( nodeKeys, nodeKey );
			}
		}
		
		return nodeKeys;
	}
	
	void MacroAction::keysFromXml( xmlNodePtr nodeKeys )
	{
		xmlNodePtr nodeKey = nodeKeys->children;
		while( nodeKey )
		{
			if( nodeKey->type == XML_ELEMENT_NODE ) 
			{
				std::string tag = (const char *) nodeKey->name;
				if( tag == JSMAPPER_XML_TAG_KEY )
				{
					uint key = KeyMap::instance()->getKeyId( xmlGetStringProp( nodeKey, BAD_CAST JSMAPPER_XML_TAG_KEY ) );
					
					std::list<std::string> lstModif = xmlGetStringListProp( nodeKey, BAD_CAST JSMAPPER_XML_TAG_MODIFIERS );
					uint modifiers = KeyMap::instance()->getModifiersIds( lstModif );
					
					addKey( key, modifiers );
				}
			}
			
			nodeKey = nodeKey->next;
		}
	}
	
	
	
	//
	// loading into device:
	//

	struct t_JSMAPPER_ACTION * MacroAction::toDeviceAction( size_t &cbBuffer ) const
	{
        // allocate parameter struct, leaving enough room for key array:
        cbBuffer = sizeof( struct t_JSMAPPER_ACTION ) + sizeof ( struct t_JSMAPPER_KEY ) * d->keys.size();
		struct t_JSMAPPER_ACTION * buffer = (struct t_JSMAPPER_ACTION *) malloc( cbBuffer );
		if( buffer )
		{
			memset( buffer, 0, cbBuffer );
			
			buffer->type = JSMAPPER_ACTION_MACRO;
			buffer->filter = filter();
			buffer->data.macro.spacing = d->spacing;
            
            int i = 0;
            KeyList::const_iterator it = d->keys.begin();
            while( it != d->keys.end() )
            {
                struct t_JSMAPPER_KEY k;
                    k.id = (*it).id;
                    k.modifiers = (*it++).modifiers;
                buffer->data.macro.keys[i++] = k;
            }
            buffer->data.macro.count = d->keys.size();
		}
		else
			JSMAPPER_LOG_ERROR( "Failed to allocate buffer!" );
		
		return buffer;
	}
}
