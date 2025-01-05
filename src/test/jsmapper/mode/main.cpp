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
 * \brief Unit test for jsmapper library's Mode class
 * \author Eduard Huguet <eduardhc@gmail.com>
 * \todo XML serialization test
 */

#include <gtest/gtest.h>

#include <jsmapper/mode.h>
#include <jsmapper/condition.h>
#include <jsmapper/profile.h>
#include <jsmapper/band.h>

#include <algorithm>

using namespace jsmapper;


static const char * MODE_NAME   = "Mode";
static const char * MODE_DESC   = "A test mode";

static const char * BTN_ID_1  = "Btn_1";
static const char * BTN_ID_2  = "Btn_2";
static const char * BTN_ID_3  = "Btn_3";
static const char * BTN_ID_4  = "Btn_4";

static const char * AXIS_ID_1  = "Axis_1";
static const char * AXIS_ID_2  = "Axis_2";
static const char * AXIS_ID_3  = "Axis_3";

static const char * ACTION_A  = "Action_A";
static const char * ACTION_B  = "Action_B";
static const char * ACTION_C  = "Action_C";


int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}


TEST( Mode, Basic )
{
    Profile profile;
    Mode * mode = new Mode( &profile );
    
        // check basic parameters:
        EXPECT_TRUE( mode->getCondition() == NULL );
        EXPECT_TRUE( mode->getParent() == NULL );
        
        mode->setName( MODE_NAME );
        EXPECT_STREQ( mode->getName().c_str(), MODE_NAME );
        
        mode->setDescription( MODE_DESC );
        EXPECT_STREQ( mode->getDescription().c_str(), MODE_DESC );
    
    profile.setRootMode( mode );    // will delete it on dtor
}


TEST( Mode, Children )
{
    Profile profile;
    Mode * root = new Mode( &profile );

        // add 3 submodes:
        Mode * mode1 = new Mode( &profile, NULL, new ButtonCondition( BTN_ID_1 ) );
        root->addChild( mode1 );
        
        Mode * mode2 = new Mode( &profile, NULL, new ButtonCondition( BTN_ID_2 ) );
        root->addChild( mode2 );
        
        Mode * mode3 = new Mode( &profile, NULL, new ButtonCondition( BTN_ID_3 ) );
        root->addChild( mode3 );
        
        
        // check them:
        EXPECT_EQ( root->getChildren().size(), 3 );
        
        ModeList::const_iterator it = root->getChildren().begin();
        EXPECT_TRUE( (*it++) == mode1 );
        EXPECT_EQ( mode1->getParent(), root );
        
        EXPECT_TRUE( (*it++) == mode2 );
        EXPECT_EQ( mode2->getParent(), root );
        
        EXPECT_TRUE( (*it++) == mode3 );
        EXPECT_EQ( mode3->getParent(), root );
            
        EXPECT_EQ( it, root->getChildren().end() );
        
        
        // remove them & check results at every step:
        root->removeChild( mode2 );
        delete mode2;
        
        EXPECT_EQ( root->getChildren().size(), 2 );
        it = root->getChildren().begin();
        EXPECT_TRUE( (*it++) == mode1 );
        EXPECT_TRUE( (*it++) == mode3 );
        EXPECT_EQ( it, root->getChildren().end() );

        
        root->removeChild( mode1 );
        delete mode1;
        
        EXPECT_EQ( root->getChildren().size(), 1 );
        it = root->getChildren().begin();
        EXPECT_TRUE( (*it++) == mode3 );
        EXPECT_EQ( it, root->getChildren().end() );
        
        
        root->removeChild( mode3 );
        delete mode3;
    
        EXPECT_EQ( root->getChildren().size(), 0 );
        it = root->getChildren().begin();
        EXPECT_EQ( it, root->getChildren().end() );
        
    profile.setRootMode( root );    // will delete it on dtor
}


TEST( Mode, Button )
{
    std::vector<std::string> buttons;
    
    Profile profile;
    Mode * root = new Mode( &profile );
    
        buttons = root->getButtons();
        EXPECT_EQ( buttons.size(), 0 ); // no mappings, initially
    
        
        // map some actions to buttons & check them:
        root->setButtonAction( BTN_ID_1, ACTION_A );
        root->setButtonAction( BTN_ID_2, ACTION_B );
        root->setButtonAction( BTN_ID_3, ACTION_C );
        
        buttons = root->getButtons();
        EXPECT_EQ( buttons.size(), 3 );
        EXPECT_TRUE( std::find( buttons.begin(), buttons.end(), BTN_ID_1 ) != buttons.end() );
        EXPECT_TRUE( std::find( buttons.begin(), buttons.end(), BTN_ID_2 ) != buttons.end() );
        EXPECT_TRUE( std::find( buttons.begin(), buttons.end(), BTN_ID_3 ) != buttons.end() );
        
        EXPECT_STREQ( root->getButtonAction( BTN_ID_1 ).c_str(), ACTION_A );
        EXPECT_STREQ( root->getButtonAction( BTN_ID_2 ).c_str(), ACTION_B );
        EXPECT_STREQ( root->getButtonAction( BTN_ID_3 ).c_str(), ACTION_C );
        EXPECT_STREQ( root->getButtonAction( BTN_ID_4 ).c_str(), "" );      // not mapped
        
        
        // override mapping for button 1:
        root->setButtonAction( BTN_ID_1, ACTION_B );
        EXPECT_STREQ( root->getButtonAction( BTN_ID_1 ).c_str(), ACTION_B );
    
        
        // remove mapping for button 2:
        root->setButtonAction( BTN_ID_2, "" );
        EXPECT_STREQ( root->getButtonAction( BTN_ID_2 ).c_str(), "" );
        
        buttons = root->getButtons();
        EXPECT_EQ( buttons.size(), 2 );
        EXPECT_TRUE( std::find( buttons.begin(), buttons.end(), BTN_ID_1 ) != buttons.end() );
        EXPECT_FALSE( std::find( buttons.begin(), buttons.end(), BTN_ID_2 ) != buttons.end() );
        EXPECT_TRUE( std::find( buttons.begin(), buttons.end(), BTN_ID_3 ) != buttons.end() );
        
        
        // remove all mappings:
        root->clearButtons();
        buttons = root->getButtons();
        EXPECT_EQ( buttons.size(), 0 );
        
    profile.setRootMode( root );    // will delete it on dtor
}


TEST( Mode, Axis )
{
    std::vector<std::string> axes;
    std::vector<Band> bands;
    
    Profile profile;
    Mode * root = new Mode( &profile );
    
        axes = root->getAxes();
        EXPECT_EQ( axes.size(), 0 ); // no mappings, initially
    
        
        // map some actions to axis bands & check them:
        root->setAxisAction( AXIS_ID_1, Band( 0, 1000 ), ACTION_A );
        root->setAxisAction( AXIS_ID_1, Band( 1001, 2000 ), ACTION_B );
        root->setAxisAction( AXIS_ID_1, Band( 2001, 3000 ), ACTION_C );
        
        axes = root->getAxes();
        EXPECT_EQ( axes.size(), 1 );
        EXPECT_TRUE( std::find( axes.begin(), axes.end(), AXIS_ID_1 ) != axes.end() );
        
        bands = root->getAxisBands( AXIS_ID_1 );
        EXPECT_EQ( bands.size(), 3 );
        
        EXPECT_STREQ( root->getAxisAction( AXIS_ID_1, Band( 0, 1000 ) ).c_str(), ACTION_A );
        EXPECT_STREQ( root->getAxisAction( AXIS_ID_1, Band( 1001, 2000 ) ).c_str(), ACTION_B );
        EXPECT_STREQ( root->getAxisAction( AXIS_ID_1, Band( 2001, 3000 ) ).c_str(), ACTION_C );
        
        EXPECT_STREQ( root->getAxisAction( AXIS_ID_1, Band( 0, 999 ) ).c_str(), "" );   // not mapped
        EXPECT_STREQ( root->getAxisAction( AXIS_ID_2, Band( 0, 1000 ) ).c_str(), "" );  // not mapped
        
        
        // remove mapping for band (1001 - 2000):
        root->setAxisAction( AXIS_ID_1, Band( 1001, 2000 ), "" );
        
        bands = root->getAxisBands( AXIS_ID_1 );
        EXPECT_EQ( bands.size(), 2 );
        
        EXPECT_STREQ( root->getAxisAction( AXIS_ID_1, Band( 0, 1000 ) ).c_str(), ACTION_A );
        EXPECT_STREQ( root->getAxisAction( AXIS_ID_1, Band( 1001, 2000 ) ).c_str(), "" );
        EXPECT_STREQ( root->getAxisAction( AXIS_ID_1, Band( 2001, 3000 ) ).c_str(), ACTION_C );
        
        
        // clear all axes:
        root->clearAxes();
        
        axes = root->getAxes();
        EXPECT_EQ( axes.size(), 0 ); // no mappings, initially
        
        bands = root->getAxisBands( AXIS_ID_1 );
        EXPECT_EQ( bands.size(), 0 );

        
    profile.setRootMode( root );    // will delete it on dtor
}
