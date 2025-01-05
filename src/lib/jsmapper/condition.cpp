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
 * \file condition.h
 * \author Eduard Huguet <eduardhc@gmail.com>
 * \brief Implementation for Condition class
 */

#include "condition.h"
#include "device.h"
#include "devicemap.h"
#include "log.h"
#include "xmlhelpers.h"

using namespace std;

namespace jsmapper
{
	Condition::Condition()
	{
	}
	
	Condition::~Condition()
	{
	}
	
	xmlNodePtr /*virtual*/ Condition::toXml() const
	{
		return xmlNewNode( NULL, BAD_CAST JSMAPPER_XML_TAG_CONDITION );
	}

	Condition * /*static*/ Condition::createFromXml( xmlNodePtr node )
	{
		Condition * cond = NULL;
		
		// create proper condition type:
		string type = xmlGetStringProp( node, BAD_CAST JSMAPPER_XML_TAG_TYPE );
		if( type == JSMAPPER_XML_TYPE_BUTTON )
		{
			cond = new ButtonCondition();
		}
		else
			JSMAPPER_LOG_ERROR( "Invalid condition type '%s'", type.c_str() );
	
		// load condition data:
		if( cond && cond->fromXml( node ) == false )
		{
			delete cond;
			cond = NULL;
		}
		
		return cond;
	}
	

	bool /*virtual*/ Condition::fromXml( xmlNodePtr node )
	{
		return true;
	}
	
	
	//
	
	
	ButtonCondition::ButtonCondition( const std::string &btnId /*= 0*/ )
		: m_btnId( btnId )
	{
	}
	
	ButtonCondition::~ButtonCondition()
	{
	}
	
	const std::string & ButtonCondition::getButton() const
	{
		return m_btnId;
	}
	
	void ButtonCondition::setButton( const std::string &btnId )
	{
		m_btnId = btnId;
	}
	
	xmlNodePtr /*virtual*/ ButtonCondition::toXml() const
	{
		xmlNodePtr nodeCond = Condition::toXml();
		if( nodeCond )
		{
			xmlNewProp( nodeCond, BAD_CAST JSMAPPER_XML_TAG_TYPE, BAD_CAST JSMAPPER_XML_TYPE_BUTTON );
			xmlNewProp( nodeCond, BAD_CAST JSMAPPER_XML_TAG_ID, BAD_CAST m_btnId.c_str() );
		}
		
		return nodeCond;
	}
	
	bool /*virtual*/ ButtonCondition::fromXml( xmlNodePtr node )
	{
		bool ret = Condition::fromXml( node );
		if( ret )
		{
			m_btnId = xmlGetStringProp( node, BAD_CAST JSMAPPER_XML_TAG_ID, m_btnId.c_str() );
		}
		
		return ret;
	}
	
    bool /*virtual*/ ButtonCondition::toDeviceCondition( Device * dev, struct t_JSMAPPER_MODE * mode )
    {
		bool ret = false;
		
		DeviceMap * map = dev->getDeviceMap();
		
        mode->condition_type = JSMAPPER_MODE_CONDITION_BUTTON;
		mode->condition.button.id = map->getButtonID( m_btnId );
		if( mode->condition.button.id != INVALID_BUTTON_ID )
		{
			ret = true;
		}
		else
			JSMAPPER_LOG_WARNING( "Unknown button ID='%s'", m_btnId.c_str() );    
		
		return ret;
    }
}
