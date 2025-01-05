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
 * \brief Unit test for jsmapper library's Action class & subclasses
 * \author Eduard Huguet <eduardhc@gmail.com>
 */

#include <gtest/gtest.h>

#include <jsmapper/keyaction.h>
#include <jsmapper/keymap.h>

using namespace jsmapper;

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

static const char * ACTION_NAME     = "Test";
static const char * ACTION_DESC     = "A test action";


//

TEST( KeyAction, Basic )
{
    size_t cbBuffer = 0;
    struct t_JSMAPPER_ACTION * pBuffer = NULL;
    
    KeyAction * action = new KeyAction( ACTION_NAME, KEY_A, JSMAPPER_MODIFIER_CTRL, false, true, ACTION_DESC );
    
        
        // check common attributes gets passed to base class:
        EXPECT_STREQ( action->getName().c_str(), ACTION_NAME );
        EXPECT_TRUE( action->filter() );
        EXPECT_STREQ( action->getDescription().c_str(), ACTION_DESC );
        
        EXPECT_EQ( action->getKey(), KEY_A );
        EXPECT_EQ( action->getModifiers(), JSMAPPER_MODIFIER_CTRL );        
        EXPECT_FALSE( action->isSingle() );

        // check conversion to device action using current data:
        pBuffer = action->toDeviceAction( cbBuffer );
            EXPECT_FALSE( pBuffer == NULL );
            EXPECT_EQ( pBuffer->type, JSMAPPER_ACTION_KEY );
            EXPECT_EQ( pBuffer->data.key.id, KEY_A );
            EXPECT_EQ( pBuffer->data.key.modifiers, JSMAPPER_MODIFIER_CTRL );
            EXPECT_FALSE( pBuffer->data.key.single );
            EXPECT_TRUE( pBuffer->filter );
        free( pBuffer );
        
        
        // change parameters & check again conversion to device action with new data:
        action->setFilter( false );
        EXPECT_FALSE( action->filter() );
        
        action->setKey( KEY_B );
        EXPECT_EQ( action->getKey(), KEY_B );

        action->setModifiers(  JSMAPPER_MODIFIER_CTRL | JSMAPPER_MODIFIER_SHIFT );
        EXPECT_EQ( action->getModifiers(), JSMAPPER_MODIFIER_CTRL | JSMAPPER_MODIFIER_SHIFT );        
        
        action->setSingle( true );
        EXPECT_TRUE( action->isSingle() );
        
        pBuffer = action->toDeviceAction( cbBuffer );
            EXPECT_FALSE( pBuffer == NULL );
            EXPECT_EQ( pBuffer->type, JSMAPPER_ACTION_KEY );
            EXPECT_EQ( pBuffer->data.key.id, KEY_B );
            EXPECT_EQ( pBuffer->data.key.modifiers, JSMAPPER_MODIFIER_CTRL | JSMAPPER_MODIFIER_SHIFT );
            EXPECT_TRUE( pBuffer->data.key.single );
            EXPECT_FALSE( pBuffer->filter );
        free( pBuffer );
        
    delete action;
}

TEST( KeyAction, Serialization )
{
    // Create action with parameters, then serialize & deserialize it. 
    // Then, check de-serialized action equals original one:
    KeyAction * action = new KeyAction( ACTION_NAME, KEY_A, JSMAPPER_MODIFIER_CTRL, false, true, ACTION_DESC );

        xmlNodePtr node = action->toXml();    
        EXPECT_TRUE( node != NULL );
    
        KeyAction * newAction = static_cast<KeyAction *>( Action::buildFromXml( node ) );
        EXPECT_TRUE( newAction != NULL );
    
            EXPECT_STREQ( action->getName().c_str(), newAction->getName().c_str() );
            EXPECT_EQ( action->filter(), newAction->filter() );
            EXPECT_STREQ( action->getDescription().c_str(), newAction->getDescription().c_str() );
            
            EXPECT_EQ( action->getKey(), newAction->getKey() );
            EXPECT_EQ( action->getModifiers(), newAction->getModifiers() );
            EXPECT_EQ( action->isSingle(), newAction->isSingle() );
        
        delete newAction;
        xmlFreeNode( node );
        
    delete action;
}
