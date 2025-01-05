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
 * \brief Unit test for jsmapper library's Condition class & subclasses
 * \author Eduard Huguet <eduardhc@gmail.com>
 * \todo XML serialization test
 */

#include <gtest/gtest.h>

#include <jsmapper/condition.h>
#include <jsmapper/keymap.h>

using namespace jsmapper;

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}


static const char * BTN_ID    = "FIRE";
static const char * BTN_ID_2  = "FIRE-2";

TEST( Condition, ButtonCondition )
{
    ButtonCondition * cond = new ButtonCondition( BTN_ID );
    
        EXPECT_STREQ( cond->getButton().c_str(), BTN_ID );
        
        cond->setButton( BTN_ID_2 );
        EXPECT_STREQ( cond->getButton().c_str(), BTN_ID_2 );
    
    delete cond;
}

