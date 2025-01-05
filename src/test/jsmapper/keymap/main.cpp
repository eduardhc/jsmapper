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
 * \brief Unit test for jsmapper library's KeyMap class
 * \author Eduard Huguet <eduardhc@gmail.com>
 * \todo XML serialization test
 */

#include <gtest/gtest.h>

#include <jsmapper/keymap.h>
#include <algorithm>

using namespace jsmapper;

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

//

TEST( KeyMap, KeySymbol )
{
    KeyMap * km = KeyMap::instance();
    
    EXPECT_STREQ( km->getKeySymbol( KEY_A ).c_str(), "A" );
    EXPECT_STREQ( km->getKeySymbol( KEY_1 ).c_str(), "1" );
    EXPECT_STREQ( km->getKeySymbol( KEY_ENTER ).c_str(), "ENTER" );
    EXPECT_STREQ( km->getKeySymbol( 99999999 ).c_str(), "" );
    
    EXPECT_EQ( km->getKeyId( "A" ), KEY_A );
    EXPECT_EQ( km->getKeyId( "1" ), KEY_1 );
    EXPECT_EQ( km->getKeyId( "ENTER" ), KEY_ENTER );
    EXPECT_EQ( km->getKeyId( "999999" ), 0 );
}


TEST( KeyMap, ModifierSymbol )
{
    KeyMap * km = KeyMap::instance();
    
    EXPECT_EQ( JSMAPPER_MODIFIER_SHIFT, JSMAPPER_MODIFIER_SHIFT_L );
    EXPECT_EQ( JSMAPPER_MODIFIER_CTRL, JSMAPPER_MODIFIER_CTRL_L );
    EXPECT_EQ( JSMAPPER_MODIFIER_ALT, JSMAPPER_MODIFIER_ALT_L );
    EXPECT_EQ( JSMAPPER_MODIFIER_META, JSMAPPER_MODIFIER_META_L );
    
    EXPECT_STREQ( km->getModifierSymbol( JSMAPPER_MODIFIER_SHIFT_L ).c_str(), JSMAPPER_XML_MODIFIER_SHIFT_L );
    EXPECT_STREQ( km->getModifierSymbol( JSMAPPER_MODIFIER_CTRL_R ).c_str(), JSMAPPER_XML_MODIFIER_CTRL_R );
    EXPECT_STREQ( km->getModifierSymbol( JSMAPPER_MODIFIER_ALT_L ).c_str(), JSMAPPER_XML_MODIFIER_ALT_L );
    EXPECT_STREQ( km->getModifierSymbol( JSMAPPER_MODIFIER_META_R ).c_str(), JSMAPPER_XML_MODIFIER_META_R );
    EXPECT_STREQ( km->getModifierSymbol( 99999999 ).c_str(), "" );
    
    EXPECT_EQ( km->getModifierId( JSMAPPER_XML_MODIFIER_SHIFT_L ), JSMAPPER_MODIFIER_SHIFT_L );
    EXPECT_EQ( km->getModifierId( JSMAPPER_XML_MODIFIER_CTRL_R ), JSMAPPER_MODIFIER_CTRL_R );
    EXPECT_EQ( km->getModifierId( JSMAPPER_XML_MODIFIER_ALT_L ), JSMAPPER_MODIFIER_ALT_L );
    EXPECT_EQ( km->getModifierId( JSMAPPER_XML_MODIFIER_META_R ), JSMAPPER_MODIFIER_META_R );
    EXPECT_EQ( km->getModifierId( "999999" ), 0 );
}


TEST( KeyMap, ModifiersSymbols )
{
    KeyMap * km = KeyMap::instance();

    std::list<std::string> symbols = km->getModifiersSymbols( JSMAPPER_MODIFIER_SHIFT_L | JSMAPPER_MODIFIER_CTRL_L );
    EXPECT_EQ( symbols.size(), 2 );
    EXPECT_TRUE( std::find(  symbols.begin(), symbols.end(), JSMAPPER_XML_MODIFIER_SHIFT_L ) != symbols.end() );
    EXPECT_TRUE( std::find(  symbols.begin(), symbols.end(), JSMAPPER_XML_MODIFIER_CTRL_L ) != symbols.end() );
    
    uint modifiers = km->getModifiersIds( symbols );
    EXPECT_EQ( modifiers, JSMAPPER_MODIFIER_SHIFT_L | JSMAPPER_MODIFIER_CTRL_L );
}


TEST( KeyMap, Buttons )
{
    KeyMap * km = KeyMap::instance();
    
    EXPECT_EQ( km->getButtonId( "LEFT" ), BTN_LEFT );
    EXPECT_EQ( km->getButtonId( "MIDDLE" ), BTN_MIDDLE );
    EXPECT_EQ( km->getButtonId( "RIGHT" ), BTN_RIGHT );
    
    EXPECT_STREQ( km->getButtonSymbol( BTN_LEFT ).c_str(), "LEFT" );
    EXPECT_STREQ( km->getButtonSymbol( BTN_MIDDLE ).c_str(), "MIDDLE" );
    EXPECT_STREQ( km->getButtonSymbol( BTN_RIGHT ).c_str(), "RIGHT" );
}
