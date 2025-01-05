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
 * \file nullaction.cpp
 * \author Eduard Huguet <eduardhc@gmail.com>
 */

#include "nullaction.h"
#include "log.h"

#include <string.h>

using namespace std;

namespace jsmapper
{
	NullAction::NullAction( const std::string& name /*= std::string()*/, 
								bool filter /*= true*/, 
								const std::string description /*= std::string()*/ )
	    : Action( NullActionType, name, filter, description )
	{
	}
	
	NullAction::~NullAction()
	{
	}
	
	
	xmlNodePtr /*virtual*/ NullAction::toXml() const
	{
		xmlNodePtr node = Action::toXml();
		if( node )
		{
			xmlNewProp( node, BAD_CAST JSMAPPER_XML_TAG_TYPE, BAD_CAST JSMAPPER_XML_TYPE_NONE );
		}
		
		return node;
	}
	
	
	bool /*virtual*/ NullAction::fromXml( xmlNodePtr node )
	{
		return Action::fromXml( node );
	}
	
	
	struct t_JSMAPPER_ACTION * /*virtual*/ NullAction::toDeviceAction( size_t &cbBuffer ) const
	{
		cbBuffer = sizeof( struct t_JSMAPPER_ACTION );
		struct t_JSMAPPER_ACTION * buffer = (struct t_JSMAPPER_ACTION *) malloc( cbBuffer );
		if( buffer )
		{
			memset( buffer, 0, cbBuffer );
			
			buffer->type 	= JSMAPPER_ACTION_NONE;
			buffer->filter 	= filter();
		}
		else
			JSMAPPER_LOG_ERROR( "Failed to allocate buffer!" );
		
		return buffer;
	}

}
