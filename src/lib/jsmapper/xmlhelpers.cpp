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
 * \file xmlhelpers.cpp
 * \brief XML helper functions
 * \author Eduard Huguet <eduardhc@gmail.com>
 */

#include "xmlhelpers.h"

#include <stdlib.h>
#include <string.h>

using namespace std;

namespace jsmapper
{
	xmlAttrPtr xmlNewIntProp( xmlNodePtr node, const xmlChar *name, int value )
	{
		char strVal[32];
		sprintf( strVal, "%i", value );
		return xmlNewProp( node, name, BAD_CAST strVal );
	}
	
	int xmlGetIntProp( xmlNodePtr node, const xmlChar *name, int defaultValue /*= 0*/ )
	{
		int result = defaultValue;
		
		xmlChar * attr = xmlGetProp( node, name );
		if( attr )
		{
			result = atoi( (const char *) attr );
			xmlFree( attr );
		}
		
		return result;
	}
	
	
	xmlAttrPtr xmlNewBoolProp( xmlNodePtr node, const xmlChar *name, bool value )
	{
		return xmlNewProp( node, name, BAD_CAST (value? "yes" : "no") );
	}
	
	bool xmlGetBoolProp( xmlNodePtr node, const xmlChar *name, bool defaultValue /*= false*/ )
	{
		bool result = defaultValue;
		
		xmlChar * attr = xmlGetProp( node, name );
		if( attr )
		{
			result = ( strcmp( (const char *) attr, "yes" ) == 0 
						|| strcmp( (const char *) attr, "true" ) == 0 
						|| strcmp( (const char *) attr, "1" ) == 0 );
			
			xmlFree( attr );
		}
		
		return result;
	}
	
	
	xmlAttrPtr xmlNewStringListProp( xmlNodePtr node, const xmlChar *name, const list<string> &value )
	{
		string allValues;
		
		list<string>::const_iterator it = value.begin();
		while( it != value.end() )
		{
			if( allValues.empty() == false )
				allValues += ';';
			allValues += *it++;
		}
		
		return xmlNewProp( node, name, BAD_CAST allValues.c_str() );
	}
	
	list<string> xmlGetStringListProp( xmlNodePtr node, const xmlChar *name )
	{
		list<string> result;

		string allValues = xmlGetStringProp( node, name );
		if( allValues.empty() == false )
		{
			char * buf = strdup( allValues.c_str() );
			
			char * value = strtok( buf, ";" );
			while( value )
			{
				result.push_back( value );
				value = strtok( NULL, ";" );
			}
			
			free( buf );
		}
		
		return result;
	}


	string xmlGetStringProp( xmlNodePtr node, const xmlChar * name, const char * defaultStr /*= NULL*/ )
	{
		string result;
		
		xmlChar * attr = xmlGetProp( node, name );
		if( attr )
		{
			result = (const char *) attr;
			xmlFree( attr );
		}
		else
		{
			if( defaultStr )
				result = defaultStr;
		}
		
		return result;
	}
	
	
	xmlNodePtr xmlNewTextElem( const xmlChar * name, const string &value )
	{
		xmlNodePtr node = xmlNewNode( NULL, name );
		
		xmlNodePtr nodeText = xmlNewText( BAD_CAST value.c_str() );
		if( nodeText )
		{
			xmlAddChild( node, nodeText );
		}
		
		return node;
	}
	
	
	string xmlGetTextElem( xmlNodePtr node, const char * defaultStr /*= NULL*/ )
	{
		string result;
		
		if( node->children 
				&& node->children->type == XML_TEXT_NODE 
				&& node->children->content )
		{
			result = (const char *) node->children->content;
		}
		else
		{
			if( defaultStr )
				result = defaultStr;
		}
		
		return result;
	}
	
}
