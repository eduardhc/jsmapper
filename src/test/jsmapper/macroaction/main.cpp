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

#include <jsmapper/macroaction.h>
#include <jsmapper/keymap.h>

using namespace jsmapper;

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

static const char * ACTION_NAME     = "Test";
static const char * ACTION_DESC     = "A test action";
static uint         MACRO_SPACING   = 1000;


TEST( MacroAction, Basic )
{
    MacroAction * action = new MacroAction( ACTION_NAME, true, ACTION_DESC );
    
        // check common attributes gets passed to base class:
        EXPECT_STREQ( action->getName().c_str(), ACTION_NAME );
        EXPECT_STREQ( action->getDescription().c_str(), ACTION_DESC );
        EXPECT_EQ( action->filter(), true );
        
        // change key spacing:    
        EXPECT_EQ( action->getSpacing(), MacroAction::DefaultSpacing );
        action->setSpacing( MACRO_SPACING );
        EXPECT_EQ( action->getSpacing(), MACRO_SPACING );
        
        // add keys:
        EXPECT_EQ( action->getKeys().size(), 0 );
        action->addKey( KEY_1 );
        action->addKey( KEY_2 );
        action->addKey( KEY_3 );
        EXPECT_EQ( action->getKeys().size(), 3 );
        
        MacroAction::KeyList::const_iterator it = action->getKeys().begin();
        EXPECT_EQ( (*it++).id, KEY_1 );
        EXPECT_EQ( (*it++).id, KEY_2 );
        EXPECT_EQ( (*it++).id, KEY_3 );
        EXPECT_EQ( it, action->getKeys().end() );

        // TODO check XML serialization
        // TODO check conversion to device action
        
        // remove keys:
        action->clearKeys();
        EXPECT_EQ( action->getKeys().size(), 0 );

    delete action;
}

TEST( MacroAction, Serialization )
{
    // Create action with parameters, then serialize & deserialize it. 
    // Then, check de-serialized action equals original one:
    MacroAction * action = new MacroAction( ACTION_NAME, true, ACTION_DESC );
    action->addKey( KEY_1 );
    action->addKey( KEY_2, JSMAPPER_MODIFIER_CTRL | JSMAPPER_MODIFIER_SHIFT );
    action->addKey( KEY_3 );
    

        xmlNodePtr node = action->toXml();    
        EXPECT_TRUE( node != NULL );
    
        MacroAction * actionNew = static_cast<MacroAction *>( Action::buildFromXml( node ) );
        EXPECT_TRUE( actionNew != NULL );
    
            EXPECT_STREQ( action->getName().c_str(), actionNew->getName().c_str() );
            EXPECT_EQ( action->filter(), actionNew->filter() );
            EXPECT_STREQ( action->getDescription().c_str(), actionNew->getDescription().c_str() );
            
            EXPECT_EQ( action->getKeys().size(), actionNew->getKeys().size() );
            MacroAction::KeyList::const_iterator it = action->getKeys().begin();
            MacroAction::KeyList::const_iterator itNew = actionNew->getKeys().begin();
            while( it != action->getKeys().end() )
            {
                const MacroAction::Key &key = *it++;
                const MacroAction::Key &keyNew = *itNew++;
                
                EXPECT_EQ( key.id, keyNew.id );
                EXPECT_EQ( key.modifiers, keyNew.modifiers );
            }
            
            EXPECT_EQ( action->getSpacing(), actionNew->getSpacing() );
        
        delete actionNew;
        xmlFreeNode( node );
        
    delete action;
}
