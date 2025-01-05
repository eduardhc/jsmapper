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
 * \brief Unit test for jsmapper library's Action base class
 * \author Eduard Huguet <eduardhc@gmail.com>
 * \todo XML serialization test
 */

#include <gtest/gtest.h>

#include <jsmapper/keyaction.h>

using namespace jsmapper;

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

static const char * ACTION_NAME     = "Test";
static const char * ACTION_DESC     = "A test action";

//

TEST( Action, Basic )
{
    Action * action = new KeyAction();
    
        action->setName( ACTION_NAME );
        EXPECT_STREQ( action->getName().c_str(), ACTION_NAME );
        
        action->setFilter( false );
        EXPECT_FALSE( action->filter() );
        
        action->setFilter( true );
        EXPECT_TRUE( action->filter() );
        
        action->setDescription( ACTION_DESC );
        EXPECT_STREQ( action->getDescription().c_str(), ACTION_DESC );
        
    delete action;
}

