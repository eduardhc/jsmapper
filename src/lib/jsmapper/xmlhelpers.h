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
 * \file xmlhelpers.h
 * \brief XML helper functions
 * \author Eduard Huguet <eduardhc@gmail.com>
 * 
 * This file contains a batch of useful XML helper functions.
 */

#ifndef __JSMAPPERLIB_XMLHELPERS_H_
#define __JSMAPPERLIB_XMLHELPERS_H_

#include "common.h"

#include <list>
#include <string>

namespace jsmapper
{
	/**
	  \brief Adds a new int-type property to an XML node
 	 */
	xmlAttrPtr xmlNewIntProp( xmlNodePtr node, const xmlChar *name, int value );
	
	/**
	 * \brief Retrieves an integer attribute value from an XML node
 	 */
	int xmlGetIntProp( xmlNodePtr node, const xmlChar *name, int defaultValue = 0 );
	
	
	/**
	 * \brief Adds a boolean property into the node
	 */
	xmlAttrPtr xmlNewBoolProp( xmlNodePtr node, const xmlChar *name, bool value );
	
	/**
	 * \brief Reads a boolean property from the node
	 */
	bool xmlGetBoolProp( xmlNodePtr node, const xmlChar *name, bool defaultValue = false );

	
	/**
	 */
	xmlAttrPtr xmlNewStringListProp( xmlNodePtr node, const xmlChar *name, const std::list<std::string> &value );
	
	/**
	 */
	std::list<std::string> xmlGetStringListProp( xmlNodePtr node, const xmlChar *name );
	
	
	
	/**
	 * \brief Retrieves an string property from an XML node
	 */
	std::string xmlGetStringProp( xmlNodePtr node, const xmlChar * name, const char * defaultStr = NULL );
	
	
	/**
	 * \brief Creates an element containing an string subnode
	 * 
	 * This function automates the process of creating an element which itself contains a text node, such as:
	 * \code
	 * 		<name>
	 * 			value
	 * 		</name>
	 * 	\endcode
	 */
	xmlNodePtr xmlNewTextElem( const xmlChar * name, const std::string &value );
	
	/**
	 * \brief Retrieves the text value contained by an element
	 * 
	 * This function automates the process or retrieving the text value contained by an element node such as:
	 * \code
	 * 		<name>
	 * 			value
	 * 		</name>
	 * 	\endcode
	 */
	std::string xmlGetTextElem( xmlNodePtr node, const char * defaultStr = NULL );
}

#endif
