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
 * \brief Unit test for jsmapper library's Profile class
 * \author Eduard Huguet <eduardhc@gmail.com>
 * \todo XML serialization test
 */

#include <gtest/gtest.h>

#include <jsmapper/profile.h>

using namespace jsmapper;

static const char * PROFILE_NAME = "Test";
static const char * PROFILE_DESC = "Test profile";

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

TEST( Profile, Basic )
{
    Profile profile;
    
    profile.setName( PROFILE_NAME );
    EXPECT_STREQ( profile.getName().c_str(), PROFILE_NAME );
    
    profile.setDescription( PROFILE_DESC );
    EXPECT_STREQ( profile.getDescription().c_str(), PROFILE_DESC );
}

