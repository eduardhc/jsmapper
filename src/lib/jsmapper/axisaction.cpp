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
 * \file axisaction.cpp
 * \brief Implementation file for axis events simulation class
 * \author Eduard Huguet <eduardhc@gmail.com>
 */

#include "axisaction.h"
#include "keymap.h"
#include "xmlhelpers.h"
#include "log.h"

#include <string.h>

namespace jsmapper
{
    const uint AxisAction::DefaultAxis = REL_X;
    const bool AxisAction::DefaultSingle = false;
    const int AxisAction::DefaultStep = 1;
    const uint AxisAction::DefaultSpacing = 100;    // ms

    /**
      * \brief Private implementation class
      */
    class AxisAction::Private
    {
    public:
        uint axis;
        int step;
        bool single;
        uint spacing;
        
    public:
        Private()
            : axis( DefaultAxis ), 
              step( DefaultStep ), 
              single( DefaultSingle ), 
              spacing( DefaultSpacing )
        {
        }
    };
    
    
    //
    
    AxisAction::AxisAction( const std::string& name /*= std::string()*/, 
                                uint axis /*= REL_X*/, 
                                int step /*= DefaultStep*/, 
                                bool single /*= DefaultSingle*/, 
                                uint spacing /*= DefaultSpacing*/, 
                                bool filter /*= true*/, 
                                const std::string description /*= std::string()*/ )
        : Action( AxisActionType, name, filter, description )
    {
        d = new Private();
        d->axis = axis;
        d->step = step;
        d->single = single;
        d->spacing = spacing;
    }
    
    AxisAction::~AxisAction()
    {
        if( d ) 
        {
            delete d;
            d = NULL;
        }
    }
    
    
    //
    
    uint AxisAction::getAxis() const
    {
        return d->axis;
    }
    
    void AxisAction::setAxis( uint axis )
    {
        d->axis = axis;
    }
    

    //     
    
    int AxisAction::getStep() const
    {
        return d->step;
    }
    
    void AxisAction::setStep( int step )
    {
        d->step = step;
    }


    //
    
    bool AxisAction::isSingle() const
	{
		return d->single;
	}
	
	void AxisAction::setSingle( bool set /*= true*/ )
	{
		d->single = set;
	}

    //
    
    uint AxisAction::getSpacing() const
    {
        return d->spacing;
    }
    
    void AxisAction::setSpacing( uint spacing )
    {
        d->spacing = spacing;
    }
    

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // Serialization
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    xmlNodePtr /*virtual*/ AxisAction::toXml() const
    {
        xmlNodePtr node = Action::toXml();
        if( node )
        {
            xmlNewProp( node, BAD_CAST JSMAPPER_XML_TAG_TYPE, BAD_CAST JSMAPPER_XML_TYPE_AXIS );
            
            xmlNewProp( node, BAD_CAST JSMAPPER_XML_TAG_AXIS, BAD_CAST KeyMap::instance()->getRelAxisSymbol( d->axis ).c_str() );
            xmlNewIntProp( node, BAD_CAST JSMAPPER_XML_TAG_STEP, d->step );
            xmlNewBoolProp( node, BAD_CAST JSMAPPER_XML_TAG_SINGLE, d->single );
            xmlNewIntProp( node, BAD_CAST JSMAPPER_XML_TAG_SPACING, d->spacing );
        }
        
        return node;
    }
    
    bool /*virtual*/ AxisAction::fromXml( xmlNodePtr node )
    {
        bool ret = Action::fromXml( node );
		if( ret )
		{
			d->axis = KeyMap::instance()->getRelAxisId( xmlGetStringProp( node, BAD_CAST JSMAPPER_XML_TAG_AXIS ) );
            d->step = xmlGetIntProp( node, BAD_CAST JSMAPPER_XML_TAG_STEP, d->step );
			d->single = xmlGetBoolProp( node, BAD_CAST JSMAPPER_XML_TAG_SINGLE, d->single );
            d->spacing = xmlGetIntProp( node, BAD_CAST JSMAPPER_XML_TAG_SPACING, d->spacing );
		}
		
		return ret;
    }

    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    // Loading into device
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    struct t_JSMAPPER_ACTION * /*virtual*/ AxisAction::toDeviceAction( size_t &cbBuffer ) const
    {
        cbBuffer = sizeof( struct t_JSMAPPER_ACTION );
		struct t_JSMAPPER_ACTION * buffer = (struct t_JSMAPPER_ACTION *) malloc( cbBuffer );
		if( buffer )
		{
			memset( buffer, 0, cbBuffer );
			
			buffer->type                = JSMAPPER_ACTION_REL;
			buffer->filter              = filter();
			buffer->data.rel.id         = getAxis();
            buffer->data.rel.step       = getStep();
            buffer->data.rel.single     = isSingle();
            buffer->data.rel.spacing    = getSpacing();
		}
		else
			JSMAPPER_LOG_ERROR( "Failed to allocate buffer!" );
		
		return buffer;
    }

}
