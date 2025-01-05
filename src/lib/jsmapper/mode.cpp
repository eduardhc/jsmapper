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
 * \file mode.cpp
 * \brief Implementation file for JSMapper mode class
 * \author Eduard Huguet <eduardhc@gmail.com>
 */

#include "mode.h"
#include "profile.h"
#include "action.h"
#include "log.h"
#include "condition.h"
#include "device.h"
#include "devicemap.h"
#include "xmlhelpers.h"
#include "band.h"

#include <stdlib.h>

#include <map>
#include <list>
#include <algorithm>

namespace jsmapper
{
	typedef std::map<std::string, std::string> ButtonsActionsMap;

	/**
	 * \brief Axis band action assignment
	 *
	 * This class represents an action assigned to an axis, through a defined band (range of values)
	 */
	class AxisBandAction
	{
	public:
		Band m_band;
		std::string m_action;

	public:
		AxisBandAction()
		{
		}

		AxisBandAction( const Band &band, const std::string &action )
			: m_band( band ),
			  m_action( action )
		{
		}
	};

	typedef std::list<AxisBandAction>                       AxisBandActionsList;
	typedef std::map<std::string, AxisBandActionsList>      AxesActionsMap;


	/**
	 * \brief Mode's private internal class
	 */
	class Mode::Private
	{
	public:
		/// Pointer to containing profile
		Profile * profile;
		/// Pointer to parent mode, or NULL if this is the root mode
		Mode * parent;
		/// Pointer to activation condition, or NULL if this is the root mode
		Condition * condition;
		/// Child submodes
		ModeList children;
		/// Mode name
		std::string 	name;
		/// Mode description
		std::string 	description;
		/// Button action mapping
		ButtonsActionsMap buttonActions;
		/// Axes action mapping
		AxesActionsMap axesActions;
		/// Mode ID, once loaded into device
		uint modeId;
		
	public:
		Private()
			: profile( NULL ),
			parent( NULL ),
			condition( NULL ), 
			modeId( 0 )
		{
		}
	};
	
	//
	
	Mode::Mode( Profile * profile, Mode * parent /*= NULL*/, Condition * cond /*= NULL*/ )
	{
		d = new Private();
		d->profile = profile;
		d->parent = parent;
		d->condition = cond;
	}
	
	Mode::~Mode()
	{
		clear();

		delete d;
		d = NULL;
	}
	
	
	//
	// attributes:
	//
	
	void Mode::clear()
	{
		d->name.clear();
		d->description.clear();
		
		// destroy submodes:
		ModeList::iterator it = d->children.begin();
		while( it != d->children.end() )
		{
			delete (*it++);
		}
		d->children.clear();
		
		clearButtons();
		clearAxes();
		
		if( d->condition )
		{
			delete d->condition;
			d->condition = NULL;
		}
	}
	
	Condition * Mode::getCondition() const
	{
		return d->condition;
	}
	
	void Mode::setCondition( Condition * condition )
	{
		if (d->condition && d->condition != condition )
			delete condition;
		
		d->condition = condition;
	}


	const std::string & Mode::getName() const
	{
		return d->name;
	}
	
	void Mode::setName( const std::string &name )
	{
		d->name = name;
	}
	
	
	const std::string & Mode::getDescription() const
	{
		return d->description;
	}
	
	void Mode::setDescription( const std::string &description )
	{
		d->description = description;
	}
	
	
	// 
	// relationships
	//
	
	Mode * Mode::getParent() const
	{
		return d->parent;
	}
	
	void Mode::setParent( Mode * parent )
	{
		d->parent = parent;
	}
	
	void Mode::addChild( Mode * mode )
	{
		if( find( d->children.begin(), d->children.end(), mode ) == d->children.end() )
		{
			mode->setParent( this );
			d->children.push_back( mode );
		}
		else
			JSMAPPER_LOG_WARNING( "Mode yet in list!" );
	}
		
	const ModeList &Mode::getChildren() const
	{
		return d->children;
	}
	
	void Mode::removeChild( Mode * mode )
	{
		ModeList::iterator it = find( d->children.begin(), d->children.end(), mode );
		if( it != d->children.end() )
		{
			mode->setParent( NULL );
			d->children.erase( it );
		}
		else
			JSMAPPER_LOG_WARNING( "Mode not in list!" );
	}
	
	
	//
	// button mapping:
	//
	
	void Mode::clearButtons()
	{
		d->buttonActions.clear();
	}
	
	void Mode::setButtonAction( const std::string &id, const std::string &action )
	{
		// set new:
		if( action.length() > 0 )
		{
			d->buttonActions[ id ] = action;
		}
		else 
		{
			std::map<std::string, std::string>::iterator it = d->buttonActions.find( id );
			if( it != d->buttonActions.end() )
				d->buttonActions.erase( it );
		}
	}

	std::string Mode::getButtonAction( const std::string &id ) const
	{
		std::string action;
		
		std::map<std::string, std::string>::const_iterator it = d->buttonActions.find( id );
		if( it != d->buttonActions.end() )
		{
			action = (*it).second;
		}
		
		return action;
	}
	

	std::vector<std::string> Mode::getButtons() const
	{
		std::vector<std::string> result;
		result.resize( d->buttonActions.size() );
		
		int pos = 0;
		ButtonsActionsMap::const_iterator it = d->buttonActions.begin();
		while( it != d->buttonActions.end() )
		{
			result[ pos++ ] = (*it++).first;
		}
		
		return result;
	}
	

	//
	// axis mapping:
	//

	void Mode::clearAxes()
	{
		d->axesActions.clear();	// this will yet destroy any associated data
	}

	void Mode::setAxisAction( const std::string &id, const Band &band, const std::string &action )
	{
		// get actions list for this axis (will be created if not existant yet):
		AxisBandActionsList &actions = d->axesActions[ id ];

		// check if band has yet been assigned, & edit if so:
		bool done = false;
		AxisBandActionsList::iterator it = actions.begin();
		while( it != actions.end() && !done )
		{
			AxisBandAction &assign = *it;
			if( assign.m_band == band )
			{
				if( action.empty() == false )
				{
					assign.m_action = action;
				}
				else
				{
					it = actions.erase( it );
					// TODO remove axis from map if list is empty...?
				}
				done = true;
			}
			else
				++it;
		}

		// else, just add it to list:
		if( done == false && action.empty() == false )
		{
			AxisBandAction assign( band, action );
			actions.push_back( assign );
		}
	}

	std::string Mode::getAxisAction( const std::string &id, const Band &band ) const
	{
		std::string result;

		AxesActionsMap::const_iterator it1 = d->axesActions.find( id );
		if( it1 != d->axesActions.end() )
		{
			const AxisBandActionsList &actions = (*it1).second;

			AxisBandActionsList::const_iterator it2 = actions.begin();
			while( it2 != actions.end() && result.empty() )
			{
				const AxisBandAction &assign = *it2++;
				if( assign.m_band == band )
				{
					result = assign.m_action;
				}
			}
		}

		return result;
	}

	std::vector<std::string> Mode::getAxes() const
	{
		std::vector<std::string> result;
		result.resize( d->axesActions.size() );

		int pos = 0;
		AxesActionsMap::const_iterator it = d->axesActions.begin();
		while( it != d->axesActions.end() )
		{
			result[ pos++ ] = (*it++).first;
		}

		return result;
	}

	std::vector<Band> Mode::getAxisBands( const std::string &id ) const
	{
		std::vector<Band> result;

		AxesActionsMap::const_iterator it1 = d->axesActions.find( id );
		if( it1 != d->axesActions.end() )
		{
			const AxisBandActionsList &actions = (*it1).second;
			result.resize( actions.size() );

			int pos = 0;
			AxisBandActionsList::const_iterator it2 = actions.begin();
			while( it2 != actions.end())
			{
				result[ pos++ ] = (*it2++).m_band;
			}
		}

		return result;
	}


	//
	// serialization
	//
	
	xmlNodePtr /*virtual*/ Mode::toXml() const
	{
		xmlNodePtr node = xmlNewNode( NULL, BAD_CAST JSMAPPER_XML_TAG_MODE );
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

			// add condition
			if( d->condition )
			{
				xmlNodePtr nodeCond = d->condition->toXml();
				if( nodeCond )
				{
					xmlAddChild( node, nodeCond );
				}
			}
			
			// add button assignments
			ButtonsActionsMap::const_iterator it1 = d->buttonActions.begin();
			while( it1 != d->buttonActions.end() )
			{
				const std::string &id = (*it1).first;
				const std::string &action = (*it1++).second;
				
				xmlNodePtr nodeBtn = xmlNewNode( NULL, BAD_CAST JSMAPPER_XML_TAG_BUTTON );
				if( nodeBtn )
				{
					xmlNewProp( nodeBtn, BAD_CAST JSMAPPER_XML_TAG_ID, BAD_CAST id.c_str() );
					xmlNewProp( nodeBtn, BAD_CAST JSMAPPER_XML_TAG_ACTION, BAD_CAST action.c_str() );
					xmlAddChild( node, nodeBtn );
				}
			}
			
			// add axes assignments
			AxesActionsMap::const_iterator it2 = d->axesActions.begin();
			while( it2 != d->axesActions.end() )
			{
				const std::string &id = (*it2).first;
				const AxisBandActionsList &actions = (*it2++).second;

				xmlNodePtr nodeAxis = xmlNewNode( NULL, BAD_CAST JSMAPPER_XML_TAG_AXIS );
				if( nodeAxis )
				{
					xmlNewProp( nodeAxis, BAD_CAST JSMAPPER_XML_TAG_ID, BAD_CAST id.c_str() );

					AxisBandActionsList::const_iterator it3 = actions.begin();
					while( it3 != actions.end() )
					{
						const AxisBandAction& assign = (*it3++);

						xmlNodePtr nodeAxisBandAction = xmlNewNode( NULL, BAD_CAST JSMAPPER_XML_TAG_BAND );
						if( nodeAxisBandAction )
						{
							xmlNewIntProp( nodeAxisBandAction, BAD_CAST JSMAPPER_XML_TAG_LOW, assign.m_band.m_low );
							xmlNewIntProp( nodeAxisBandAction, BAD_CAST JSMAPPER_XML_TAG_HIGH, assign.m_band.m_high );
							xmlNewProp( nodeAxisBandAction, BAD_CAST JSMAPPER_XML_TAG_ACTION, BAD_CAST assign.m_action.c_str() );

							xmlAddChild( nodeAxis, nodeAxisBandAction );
						}
					}

					xmlAddChild( node, nodeAxis );
				}
			}

			
			// add children modes:
			ModeList::const_iterator itModes = d->children.begin();
			while( itModes != d->children.end() )
			{
				Mode * subMode = *itModes++;
				
				xmlNodePtr nodeSubMode = subMode->toXml();
				if( nodeSubMode )
				{
					xmlAddChild( node, nodeSubMode );
				}
			}
		}
		 
		return node;
	}
		
		
	bool /*virtual*/ Mode::fromXml( xmlNodePtr node )
	{
		bool ret = true;
		
		d->name = xmlGetStringProp( node, BAD_CAST JSMAPPER_XML_TAG_NAME );
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
				else if( tag == JSMAPPER_XML_TAG_MODE )
				{
					// read submode:
					Mode * subMode = new Mode( d->profile, this );
					if( subMode->fromXml( subNode ) )
					{
						addChild( subMode );
					}
					else
					{
						JSMAPPER_LOG_ERROR( "Failed to load child mode!" );
						delete subMode;
						subMode = NULL;
					}
					
				}
				else if( tag == JSMAPPER_XML_TAG_CONDITION )
				{
					// read condition:
					Condition * cond = Condition::createFromXml( subNode );
					if( cond )
					{
						setCondition( cond );
					}
					
				}
				else if( tag == JSMAPPER_XML_TAG_BUTTON )
				{
					// read button assignment:
					std::string id = xmlGetStringProp( subNode, BAD_CAST JSMAPPER_XML_TAG_ID );
					std::string action = xmlGetStringProp( subNode, BAD_CAST JSMAPPER_XML_TAG_ACTION );
					if( id.empty() == false && action.empty() == false )
					{
						setButtonAction( id, action );
					}

				}
				else if( tag == JSMAPPER_XML_TAG_AXIS )
				{
					// read axis assignment:
					std::string id = xmlGetStringProp( subNode, BAD_CAST JSMAPPER_XML_TAG_ID );
					if( id.empty() == false )
					{
						xmlNodePtr bandNode = subNode->children;
						while( bandNode && ret )
						{
							if( bandNode->type == XML_ELEMENT_NODE) 
							{
								std::string tag = (const char *) bandNode->name;
								if( tag == JSMAPPER_XML_TAG_BAND )
								{
									std::string action = xmlGetStringProp( bandNode, BAD_CAST JSMAPPER_XML_TAG_ACTION );
									int low = xmlGetIntProp( bandNode, BAD_CAST JSMAPPER_XML_TAG_LOW );
									int high = xmlGetIntProp( bandNode, BAD_CAST JSMAPPER_XML_TAG_HIGH );
									if( action.empty() == false )
									{
										setAxisAction( id, Band( low, high ), action );
									}
								}
							}

							bandNode = bandNode->next;
						}
					}

				}
			}
			
			subNode = subNode->next;
		}
		
		return ret;
	}
	
	
    //
    // Loading into the device
    //
    
    bool Mode::toDevice( Device * dev )
    {
        bool result = true;
        
        // create new mode into device, first:
        if( d->modeId == 0 && d->parent != NULL )   // else is root mode 
        {
            d->modeId = dev->addMode( d->condition, d->parent->getModeId() );
			if( d->modeId != 0 ) 
			{
				JSMAPPER_LOG_INFO( "Mode \"%s\" created -> ID=%u", d->name.c_str(), d->modeId );
			} 
			else 
			{
                JSMAPPER_LOG_ERROR( "Failed to create new mode - aborting!" );
                result = false;
            }
		}

        if( result )
        {
			// load button assignments:
			result = buttonsToDevice( dev );
        }
        
		if( result )
		{
			// load axes assignments:
			result = axesToDevice( dev );
		}

        if( result )
        {
            // finally, load submodes:
            ModeList::iterator it = d->children.begin();
            while( it != d->children.end() && result )
            {
                Mode * mode = *it++;
                result = mode->toDevice( dev );
            }
        }
        
        
        // don't reuse mode ID after loading: the library will clear the device before reloading a profile, so 
        // we'll need to recreate it:
        d->modeId = 0;
        
        return result;
    }
    
    
    uint Mode::getModeId() const
    {
        return d->modeId;
    }


	bool Mode::buttonsToDevice( Device * dev )
	{
		bool result = true;

		ButtonsActionsMap::const_iterator it = d->buttonActions.begin();
		while( it != d->buttonActions.end() && result )
		{
			const std::string &id = (*it).first;
			const std::string &action = (*it++).second;

			// resolve button ID:
			DeviceMap * map = dev->getDeviceMap();
			ButtonID realId = map->getButtonID( id );
			if( realId != INVALID_BUTTON_ID )
			{
				// resolve action name:
				Action * pAction = d->profile->getAction( action );
				if( pAction )
				{
					// ok, load into device:
					result = dev->setButtonAction( d->modeId, realId, pAction );
				}
				else
					JSMAPPER_LOG_ERROR( "Unknown action '%s'!", action.c_str() );
			}
			else
				JSMAPPER_LOG_ERROR( "Unknown button ID=%s!", id.c_str() );
		}

		return result;
	}

	bool Mode::axesToDevice( Device * dev )
	{
		bool result = true;

		AxesActionsMap::const_iterator it = d->axesActions.begin();
		while( it != d->axesActions.end() && result )
		{
			const std::string &id = (*it).first;
			const AxisBandActionsList &actions = (*it++).second;

			// resolve axis ID:
			DeviceMap * map = dev->getDeviceMap();
			AxisID realId = map->getAxisID( id );
			if( realId != INVALID_AXIS_ID )
			{
				// Ok, enter action bands:
				AxisBandActionsList::const_iterator it2 = actions.begin();
				while( it2 != actions.end() )
				{
					const AxisBandAction &assign = (*it2++);

					// resolve action name:
					Action * pAction = d->profile->getAction( assign.m_action );
					if( pAction )
					{
						// OK, load into device:
						result = dev->setAxisAction( d->modeId, realId, assign.m_band, pAction );
					}
					else
						JSMAPPER_LOG_ERROR( "Unknown action '%s'!", assign.m_action.c_str() );
				}
			}
			else
				JSMAPPER_LOG_ERROR( "Unknown axis ID=%s!", id.c_str() );
		}

		return result;
	}
}

