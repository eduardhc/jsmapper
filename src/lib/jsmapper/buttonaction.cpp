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
 * \file buttonaction.cpp
 * \brief Implementation file for button events simulation class
 * \author Eduard Huguet <eduardhc@gmail.com>
 */

#include "buttonaction.h"
#include "keymap.h"
#include "xmlhelpers.h"
#include "log.h"

#include <string.h>

using namespace std;

namespace jsmapper
{
    const uint /*static*/ ButtonAction::DefaultButton = BTN_LEFT;
    const uint /*static*/ ButtonAction::DefaultModifiers = 0;
    const bool /*static*/ ButtonAction::DefaultSingle = false;
    
	class ButtonAction::Private
	{
	public:
		uint button;
		uint modifiers;
		bool single;
		
	public:
		Private()
			: button( DefaultButton ), 
              modifiers( DefaultModifiers ), 
              single( DefaultSingle )
		{
		}
	};
	
	
	ButtonAction::ButtonAction( const string& name, 
                                uint button /*= DefaultButton*/, 
                                uint modifiers /*= DefaultModifiers*/, 
                                bool single /*= false*/, 
                                bool filter /*= true*/, 
                                const string description /*= string()*/ )
		: Action( ButtonActionType, name, filter, description )
	{
		d = new Private();
		d->button = button;
		d->modifiers = modifiers;
		d->single = single;
	}
	
	ButtonAction::~ButtonAction()
	{
		if( d )
		{
			delete d;
			d = NULL;
		}
	}

	
	//
	
	uint ButtonAction::getButton() const
	{
		return d->button;
	}
	
	void ButtonAction::setButton( uint button )
	{
		d->button = button;
	}

	//
	
	uint ButtonAction::getModifiers() const
	{
		return d->modifiers;
	}
	
	void ButtonAction::setModifiers( uint modifiers )
	{
		d->modifiers = modifiers;
	}

	//
	
	bool ButtonAction::isSingle() const
	{
		return d->single;
	}
	
	void ButtonAction::setSingle( bool set /*= true*/ )
	{
		d->single = set;
	}

	//

	xmlNodePtr /*virtual*/ ButtonAction::toXml() const
	{
		xmlNodePtr node = Action::toXml();
		if( node )
		{
			xmlNewProp( node, BAD_CAST JSMAPPER_XML_TAG_TYPE, BAD_CAST JSMAPPER_XML_TYPE_BUTTON );
			xmlNewProp( node, BAD_CAST JSMAPPER_XML_TAG_BUTTON, BAD_CAST KeyMap::instance()->getButtonSymbol( d->button ).c_str() );
			
			list<string> lstModif = KeyMap::instance()->getModifiersSymbols( d->modifiers );
			xmlNewStringListProp( node, BAD_CAST JSMAPPER_XML_TAG_MODIFIERS, lstModif );
			
			xmlNewBoolProp( node, BAD_CAST JSMAPPER_XML_TAG_SINGLE, d->single );
		}
		
		return node;
	}
	
	
	bool /*virtual*/ ButtonAction::fromXml( xmlNodePtr node )
	{
		bool ret = Action::fromXml( node );
		if( ret )
		{
			d->button = KeyMap::instance()->getButtonId( xmlGetStringProp( node, BAD_CAST JSMAPPER_XML_TAG_BUTTON ) );
			
			list<string> lstModif = xmlGetStringListProp( node, BAD_CAST JSMAPPER_XML_TAG_MODIFIERS );
			d->modifiers = KeyMap::instance()->getModifiersIds( lstModif );

			d->single = xmlGetBoolProp( node, BAD_CAST JSMAPPER_XML_TAG_SINGLE, d->single );
		}
		
		return ret;
	}

	
	//
	// action loading helpers:
	//
	
	struct t_JSMAPPER_ACTION * /*virtual*/ ButtonAction::toDeviceAction( size_t &cbBuffer ) const
	{
		cbBuffer = sizeof( struct t_JSMAPPER_ACTION );
		struct t_JSMAPPER_ACTION * buffer = (struct t_JSMAPPER_ACTION *) malloc( cbBuffer );
		if( buffer )
		{
			memset( buffer, 0, cbBuffer );
			
            // sent as a KEY-type action too:
			buffer->type                = JSMAPPER_ACTION_KEY;
			buffer->filter              = filter();
			buffer->data.key.id         = getButton();
			buffer->data.key.modifiers	= getModifiers();
            buffer->data.key.single     = isSingle();
		}
		else
			JSMAPPER_LOG_ERROR( "Failed to allocate buffer!" );
		
		return buffer;
	}
	
}
