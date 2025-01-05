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
 * \brief Unit test for jsmapper library's ButtonAction class
 * \author Eduard Huguet <eduardhc@gmail.com>
 */

#include <gtest/gtest.h>

#include <jsmapper/buttonaction.h>

using namespace jsmapper;

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

static const char * ACTION_NAME         = "Test";
static const char * ACTION_DESC         = "Mouse buttons test action";
static bool         ACTION_FILTER       = false;
static int          ACTION_BTN_ID       = BTN_MIDDLE;
static int          ACTION_MODIFIERS    = JSMAPPER_MODIFIER_CTRL;
static int          ACTION_SINGLE       = true;

TEST( ButtonAction, Basic )
{
    // check ctor parameters get passed correctly
    ButtonAction * action = new ButtonAction( ACTION_NAME, 
                                                ACTION_BTN_ID, 
                                                ACTION_MODIFIERS, 
                                                ACTION_SINGLE, 
                                                ACTION_FILTER, 
                                                ACTION_DESC );
    
        EXPECT_STREQ( action->getName().c_str(), ACTION_NAME );
        EXPECT_EQ( action->filter(), ACTION_FILTER );
        EXPECT_STREQ( action->getDescription().c_str(), ACTION_DESC );
        
        EXPECT_EQ( action->getButton(), ACTION_BTN_ID );
        EXPECT_EQ( action->getModifiers(), ACTION_MODIFIERS );
        EXPECT_EQ( action->isSingle(), ACTION_SINGLE );
    
    delete action;
}


TEST( ButtonAction, SetParams )
{
    ButtonAction * action = new ButtonAction( ACTION_NAME );
    
        // check default values:
        EXPECT_EQ( action->getButton(), ButtonAction::DefaultButton );
        EXPECT_EQ( action->getModifiers(), ButtonAction::DefaultModifiers );
        EXPECT_EQ( action->isSingle(), ButtonAction::DefaultSingle );
        
        // change button, type, etc...
        action->setButton( ACTION_BTN_ID );
        EXPECT_EQ( action->getButton(), ACTION_BTN_ID );
        
        action->setModifiers( ACTION_MODIFIERS );
        EXPECT_EQ( action->getModifiers(), ACTION_MODIFIERS );
        
        action->setSingle( ACTION_SINGLE );
        EXPECT_EQ( action->isSingle(), ACTION_SINGLE );
        
    delete action;
}


TEST( ButtonAction, Device )
{
    ButtonAction * action = new ButtonAction( ACTION_NAME );
        
        action->setButton( ACTION_BTN_ID );
        action->setModifiers( ACTION_MODIFIERS );
        action->setSingle( ACTION_SINGLE );
        action->setFilter( ACTION_FILTER );
        
        size_t cbBuf = 0;
        struct t_JSMAPPER_ACTION * pBuf = action->toDeviceAction( cbBuf );
        
            EXPECT_TRUE( pBuf != NULL );
            EXPECT_EQ( pBuf->type, JSMAPPER_ACTION_KEY );
            EXPECT_EQ( pBuf->filter, ACTION_FILTER );
            EXPECT_EQ( pBuf->data.key.id, ACTION_BTN_ID );
            EXPECT_EQ( pBuf->data.key.modifiers, ACTION_MODIFIERS );
            EXPECT_EQ( pBuf->data.key.single, ACTION_SINGLE );
        
        free( pBuf );
    
    delete action;
}


TEST( ButtonAction, Serialization )
{
    // Create action with parameters, then serialize & deserialize it. 
    // Then, check de-serialized action equals original one:
    ButtonAction * action = new ButtonAction( ACTION_NAME, 
                                                ACTION_BTN_ID, 
                                                ACTION_MODIFIERS, 
                                                ACTION_SINGLE, 
                                                ACTION_FILTER, 
                                                ACTION_DESC );
    
        xmlNodePtr node = action->toXml();    
        EXPECT_TRUE( node != NULL );
    
        ButtonAction * newAction = static_cast<ButtonAction *>( Action::buildFromXml( node ) );
        EXPECT_TRUE( newAction != NULL );
    
            EXPECT_STREQ( action->getName().c_str(), newAction->getName().c_str() );
            EXPECT_EQ( action->filter(), newAction->filter() );
            EXPECT_STREQ( action->getDescription().c_str(), newAction->getDescription().c_str() );
            
            EXPECT_EQ( action->getButton(), newAction->getButton() );
            EXPECT_EQ( action->getModifiers(), newAction->getModifiers() );
            EXPECT_EQ( action->isSingle(), newAction->isSingle() );
        
        delete newAction;
        xmlFreeNode( node );
        
    delete action;
}
