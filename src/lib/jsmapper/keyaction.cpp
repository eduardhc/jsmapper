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
 * \file keyaction.cpp
 * \brief Implementation file for key events simulation class
 * \author Eduard Huguet <eduardhc@gmail.com>
 */

#include "keyaction.h"
#include "keymap.h"
#include "xmlhelpers.h"
#include "log.h"

#include <string.h>

using namespace std;

namespace jsmapper
{
	class KeyAction::Private
	{
	public:
		uint key;
		uint modifiers;
		bool single;
		
	public:
		Private()
			: key( 0 ), modifiers( 0 ), single( false )
		{
		}
	};
	
	
	KeyAction::KeyAction( const string& name, 
                            uint key, uint modifiers /*= 0*/, 
                            bool single /*= false*/, 
                            bool filter /*= true*/, 
                            const string description /*= string()*/ )
		: Action( KeyActionType, name, filter, description )
	{
		d = new Private();
		d->key = key;
		d->modifiers = modifiers;
		d->single = single;
	}
	
	KeyAction::~KeyAction()
	{
		if( d )
		{
			delete d;
			d = NULL;
		}
	}

	
	//
	
	uint KeyAction::getKey() const
	{
		return d->key;
	}
	
	void KeyAction::setKey( uint key )
	{
		d->key = key;
	}

	//
	
	uint KeyAction::getModifiers() const
	{
		return d->modifiers;
	}
	
	void KeyAction::setModifiers( uint modifiers )
	{
		d->modifiers = modifiers;
	}

	//
	
	bool KeyAction::isSingle() const
	{
		return d->single;
	}
	
	void KeyAction::setSingle( bool set /*= true*/ )
	{
		d->single = set;
	}

	//

	xmlNodePtr /*virtual*/ KeyAction::toXml() const
	{
		xmlNodePtr node = Action::toXml();
		if( node )
		{
			xmlNewProp( node, BAD_CAST JSMAPPER_XML_TAG_TYPE, BAD_CAST JSMAPPER_XML_TYPE_KEY );
			xmlNewProp( node, BAD_CAST JSMAPPER_XML_TAG_KEY, BAD_CAST KeyMap::instance()->getKeySymbol( d->key ).c_str() );
			
			list<string> lstModif = KeyMap::instance()->getModifiersSymbols( d->modifiers );
			xmlNewStringListProp( node, BAD_CAST JSMAPPER_XML_TAG_MODIFIERS, lstModif );
			
			xmlNewBoolProp( node, BAD_CAST JSMAPPER_XML_TAG_SINGLE, d->single );
		}
		
		return node;
	}
	
	
	bool /*virtual*/ KeyAction::fromXml( xmlNodePtr node )
	{
		bool ret = Action::fromXml( node );
		if( ret )
		{
			d->key = KeyMap::instance()->getKeyId( xmlGetStringProp( node, BAD_CAST JSMAPPER_XML_TAG_KEY ) );
			
			list<string> lstModif = xmlGetStringListProp( node, BAD_CAST JSMAPPER_XML_TAG_MODIFIERS );
			d->modifiers = KeyMap::instance()->getModifiersIds( lstModif );

			d->single = xmlGetBoolProp( node, BAD_CAST JSMAPPER_XML_TAG_SINGLE, d->single );
		}
		
		return ret;
	}

	
	//
	// action loading helpers:
	//
	
	struct t_JSMAPPER_ACTION * /*virtual*/ KeyAction::toDeviceAction( size_t &cbBuffer ) const
	{
		cbBuffer = sizeof( struct t_JSMAPPER_ACTION );
		struct t_JSMAPPER_ACTION * buffer = (struct t_JSMAPPER_ACTION *) malloc( cbBuffer );
		if( buffer )
		{
			memset( buffer, 0, cbBuffer );
			
			buffer->type                = JSMAPPER_ACTION_KEY;
			buffer->filter              = filter();
			buffer->data.key.id         = getKey();
			buffer->data.key.modifiers	= getModifiers();
            buffer->data.key.single     = isSingle();
		}
		else
			JSMAPPER_LOG_ERROR( "Failed to allocate buffer!" );
		
		return buffer;
	}
	
}
