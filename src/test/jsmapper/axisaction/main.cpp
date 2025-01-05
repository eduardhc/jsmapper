/**
 * Copyright 2013 Eduard Huguet Cuadrench
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2.1 of the License.
 * 
 * Foobar is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * 
 * \file main.cpp
 * \brief Unit test for jsmapper library's AxisAction class
 * \author Eduard Huguet <eduardhc@gmail.com>
 */

#include <gtest/gtest.h>

#include <jsmapper/axisaction.h>

using namespace jsmapper;

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

static const char * ACTION_NAME     = "Test";
static const char * ACTION_DESC     = "An axis test action";
static bool         ACTION_FILTER   = false;

static uint             AXIS_ID         = REL_Y;
static int              AXIS_STEP       = 5;
static bool             AXIS_SINGLE     = true;
static uint             AXIS_SPACING    = 10;


TEST( AxisAction, Basic )
{
    // check ctor parameters get passed correctly
    AxisAction * action = new AxisAction( ACTION_NAME, 
                                            AXIS_ID, AXIS_STEP, 
                                            AXIS_SINGLE, 
                                            AXIS_SPACING, 
                                            ACTION_FILTER, 
                                            ACTION_DESC );
    
        EXPECT_STREQ( action->getName().c_str(), ACTION_NAME );
        EXPECT_EQ( action->filter(), ACTION_FILTER );
        EXPECT_STREQ( action->getDescription().c_str(), ACTION_DESC );
        
        EXPECT_EQ( action->getAxis(), AXIS_ID );
        EXPECT_EQ( action->getStep(), AXIS_STEP );
        EXPECT_EQ( action->isSingle(), AXIS_SINGLE );
        EXPECT_EQ( action->getSpacing(), AXIS_SPACING );
    
    delete action;
}


TEST( AxisAction, SetParams )
{
    AxisAction * action = new AxisAction( ACTION_NAME );
    
        // check default values:
        EXPECT_EQ( action->getAxis(), AxisAction::DefaultAxis );
        EXPECT_EQ( action->getStep(), AxisAction::DefaultStep );
        EXPECT_EQ( action->isSingle(), AxisAction::DefaultSingle );
        EXPECT_EQ( action->getSpacing(), AxisAction::DefaultSpacing );
        
        // change axis, type, etc...
        action->setAxis( AXIS_ID );
        EXPECT_EQ( action->getAxis(), AXIS_ID );
        
        action->setSingle( AXIS_SINGLE );
        EXPECT_EQ( action->isSingle(), AXIS_SINGLE );
        
        action->setStep( AXIS_STEP );
        EXPECT_EQ( action->getStep(), AXIS_STEP );
        
        action->setSpacing( AXIS_SPACING );
        EXPECT_EQ( action->getSpacing(), AXIS_SPACING );
    
    delete action;
}


TEST( AxisAction, Device )
{
    AxisAction * action = new AxisAction( ACTION_NAME );
        
        action->setFilter( false );
        action->setAxis( AXIS_ID );
        action->setStep( AXIS_STEP );
        action->setSingle( AXIS_SINGLE );
        action->setSpacing( AXIS_SPACING );
        
        size_t cbBuf = 0;
        struct t_JSMAPPER_ACTION * pBuf = action->toDeviceAction( cbBuf );
        
            EXPECT_TRUE( pBuf != NULL );
            EXPECT_EQ( pBuf->type, JSMAPPER_ACTION_REL );
            EXPECT_EQ( pBuf->filter, ACTION_FILTER );
            EXPECT_EQ( pBuf->data.rel.id, AXIS_ID );
            EXPECT_EQ( pBuf->data.rel.step, AXIS_STEP );
            EXPECT_EQ( pBuf->data.rel.single, AXIS_SINGLE );
            EXPECT_EQ( pBuf->data.rel.spacing, AXIS_SPACING );
        
        free( pBuf );
    
    delete action;
}


TEST( AxisAction, Serialization )
{
    // Create action with parameters, then serialize & deserialize it. 
    // Then, check de-serialized action equals original one:
    AxisAction * action = new AxisAction( ACTION_NAME, 
                                            AXIS_ID, AXIS_STEP, 
                                            AXIS_SINGLE, 
                                            AXIS_SPACING, 
                                            ACTION_FILTER, 
                                            ACTION_DESC );
    

        xmlNodePtr node = action->toXml();    
        EXPECT_TRUE( node != NULL );
    
        AxisAction * newAction = static_cast<AxisAction *>( Action::buildFromXml( node ) );
        EXPECT_TRUE( newAction != NULL );
    
            EXPECT_STREQ( action->getName().c_str(), newAction->getName().c_str() );
            EXPECT_EQ( action->filter(), newAction->filter() );
            EXPECT_STREQ( action->getDescription().c_str(), newAction->getDescription().c_str() );
            
            EXPECT_EQ( action->getAxis(), newAction->getAxis() );
            EXPECT_EQ( action->getStep(), newAction->getStep() );
            EXPECT_EQ( action->isSingle(), newAction->isSingle() );
            EXPECT_EQ( action->getSpacing(), newAction->getSpacing() );
        
        delete newAction;
        xmlFreeNode( node );
        
    delete action;
}
