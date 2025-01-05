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
 * \file action.cpp
 * \brief Implementation file for base Action class
 * \author Eduard Huguet <eduardhc@gmail.com>
 */

#include "action.h"

#include "axisaction.h"
#include "buttonaction.h"
#include "keyaction.h"
#include "macroaction.h"
#include "nullaction.h"

#include "xmlhelpers.h"
#include "log.h"

using namespace std;

namespace jsmapper
{
	/**
	 * \brief Action's private internal class
	 */
	class Action::Private
	{
	public:
		ActionType type;
		std::string name;
		bool filter;
		std::string description;
		
	public:
		Private()
			: type( UnknownActionType ),
			  filter( false )
		{
		}
	};
	
	
	//
	
	Action::Action( ActionType type, const std::string &name, bool filter /*= true*/, const std::string description /*= std::string()*/ )
	{
		d = new Private();
		d->type = type;
		d->name = name;
		d->filter = filter;
		d->description = description;
	}
	
	Action::~Action()
	{
		if( d )
		{
			delete d;
			d = NULL;
		}
	}


	//
	
	ActionType Action::getType() const
	{
		return d->type;
	}
	
	
	//
	
	const std::string & Action::getName() const
	{
		return d->name;
	}
	
	void Action::setName( const std::string &name )
	{
		d->name = name;
	}


	bool Action::filter() const
	{
		return d->filter;
	}
		
	void Action::setFilter( bool filter /*= true*/ )
	{
		d->filter = filter;
	}


	const std::string & Action::getDescription() const
	{
		return d->description;
	}
	
	void Action::setDescription( const std::string &description )
	{
		d->description = description;
	}
	
	
	xmlNodePtr /*virtual*/ Action::toXml() const
	{
		xmlNodePtr node = xmlNewNode( NULL, BAD_CAST JSMAPPER_XML_TAG_ACTION );
		if( node )
		{
			// add name & description:
			xmlNewProp( node, BAD_CAST JSMAPPER_XML_TAG_NAME, BAD_CAST d->name.c_str() );
			if( d->description.empty() == false )
			{
				xmlNodePtr nodeDesc = xmlNewTextElem( BAD_CAST JSMAPPER_XML_TAG_DESCRIPTION, d->description );
				if( nodeDesc )
				{
					xmlAddChild( node, nodeDesc );
				}
			}
			
			xmlNewBoolProp( node, BAD_CAST JSMAPPER_XML_TAG_FILTER, d->filter );
		}
		
		return node;
	}
	
	
	bool /*virtual*/ Action::fromXml( xmlNodePtr  node )
	{
		bool ret = true;
		
		d->name = xmlGetStringProp( node, BAD_CAST JSMAPPER_XML_TAG_NAME, d->name.c_str() );
		d->filter = xmlGetBoolProp( node, BAD_CAST JSMAPPER_XML_TAG_FILTER, d->filter );
		
		xmlNodePtr subNode = node->children;
		while( subNode && ret )
		{
			if( subNode->type == XML_ELEMENT_NODE) 
			{
				std::string tag = (const char *) subNode->name;
				if( tag == JSMAPPER_XML_TAG_DESCRIPTION )
				{
					d->description = xmlGetTextElem( subNode );
				}
			}
			
			subNode = subNode->next;
		}
		
		return ret;
	}
		
	
	Action * /*static*/ Action::buildFromXml( xmlNodePtr node )
	{
		Action * action = NULL;
		
		// create proper condition type:
		string type = xmlGetStringProp( node, BAD_CAST JSMAPPER_XML_TAG_TYPE );
        if( type == JSMAPPER_XML_TYPE_AXIS )
		{
			action = new AxisAction();
		}
        else if( type == JSMAPPER_XML_TYPE_BUTTON )
		{
			action = new ButtonAction();
		}
		else if( type == JSMAPPER_XML_TYPE_KEY )
		{
			action = new KeyAction();
		}
		else if( type == JSMAPPER_XML_TYPE_MACRO )
		{
			action = new MacroAction();
		}
		else if( type == JSMAPPER_XML_TYPE_NONE )
		{
			action = new NullAction();
		}
		else
			JSMAPPER_LOG_ERROR( "Invalid action type '%s'", type.c_str() );
	
		// load condition data:
		if( action && action->fromXml( node ) == false )
		{
			delete action;
			action = NULL;
		}
		
		return action;
	}
}

