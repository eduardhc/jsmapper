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
 * \file keymap.cpp
 * \brief Implementation file for key map helper class
 * \author Eduard Huguet <eduardhc@gmail.com>
 */

#include "keymap.h"
#include "log.h"

#include <map>
using namespace std;

namespace jsmapper
{
	class KeyMap::Private
	{
	public:
		map<uint, string> keySyms;
		map<string, uint> keyIds;
        map<uint, string> btnSyms;
		map<string, uint> btnIds;
		map<uint, string> modSyms;
		map<string, uint> modIds;
        map<uint, string> relSyms;
		map<string, uint> relIds;
	};
	
	
	KeyMap KeyMap::theMap;
	
	KeyMap * /*static*/ KeyMap::instance()
	{
		return &theMap;
	}
	
	KeyMap::KeyMap()
	{
		d = new Private();
		
		init();
	}
	
	KeyMap::~KeyMap()
	{
		delete d;
		d = NULL;
	}

    
    //	
    // keys
    // 
    
    list<string> KeyMap::getKeySymbols() const
    {
        list<string> syms;
                
        map<uint, string>::const_iterator it = d->keySyms.begin();
        while( it != d->keySyms.end() )
        {
            syms.push_back( (*it++).second );
        }
        
        return syms;
    }
	
    string KeyMap::getKeySymbol( uint keyId ) const
	{
		string keySym;
		
		map<uint, string>::const_iterator it = d->keySyms.find( keyId );
		if( it != d->keySyms.end() )
		{
			keySym = (*it).second;
		}
		else
			JSMAPPER_LOG_WARNING( "Unrecognized key ID '0x%x'", keyId );
		
		return keySym;
	}
	
	uint KeyMap::getKeyId( const string &keySym ) const
	{
		uint keyId = 0;
		
		map<string, uint>::const_iterator it = d->keyIds.find( keySym );
		if( it != d->keyIds.end() )
		{
			keyId = (*it).second;
		}
		else
			JSMAPPER_LOG_WARNING( "Unrecognized key symbol '%s'", keySym.c_str() );
		
		return keyId;
	}
	
    
    //	
    // buttons
    // 
    
    list<string> KeyMap::getButtonsSymbols() const
    {
        list<string> syms;
                
        map<uint, string>::const_iterator it = d->btnSyms.begin();
        while( it != d->btnSyms.end() )
        {
            syms.push_back( (*it++).second );
        }
        
        return syms;
    }
    
    string KeyMap::getButtonSymbol( uint btnId ) const
    {
        string btnSym;
		
		map<uint, string>::const_iterator it = d->btnSyms.find( btnId );
		if( it != d->btnSyms.end() )
		{
			btnSym = (*it).second;
		}
		else
			JSMAPPER_LOG_WARNING( "Unrecognized button ID '0x%x'", btnId );
		
		return btnSym;
    }

    uint KeyMap::getButtonId( const string &btnSym ) const
    {
        uint btnId = 0;
		
		map<string, uint>::const_iterator it = d->btnIds.find( btnSym );
		if( it != d->btnIds.end() )
		{
			btnId = (*it).second;
		}
		else
			JSMAPPER_LOG_WARNING( "Unrecognized button symbol '%s'", btnSym.c_str() );
		
		return btnId;
    }
    
    
    //	
    // modifiers
    // 
    
	string KeyMap::getModifierSymbol( uint modId ) const
	{
		string modSym;
		
		map<uint, string>::const_iterator it = d->modSyms.find( modId );
		if( it != d->modSyms.end() )
		{
			modSym = (*it).second;
		}
		else
			JSMAPPER_LOG_WARNING( "Unrecognized key modifier ID '0x%x'", modId );
		
		return modSym;
	}
	
	uint KeyMap::getModifierId( const string &modSym ) const
	{
		uint modId = 0;
		
		map<string, uint>::const_iterator it = d->modIds.find( modSym );
		if( it != d->modIds.end() )
		{
			modId = (*it).second;
		}
		else
			JSMAPPER_LOG_WARNING( "Unrecognized key modifier symbol '%s'", modSym.c_str() );
		
		return modId;
	}
	

    static uint g_modifiers[] =
	{
		JSMAPPER_MODIFIER_SHIFT_L,
		JSMAPPER_MODIFIER_SHIFT_R,
		JSMAPPER_MODIFIER_CTRL_L,
		JSMAPPER_MODIFIER_CTRL_R,
		JSMAPPER_MODIFIER_ALT_L,
		JSMAPPER_MODIFIER_ALT_R,
		JSMAPPER_MODIFIER_META_L,
		JSMAPPER_MODIFIER_META_R,
		0
	};


	//

	list<string> KeyMap::getModifiersSymbols( uint modifiers ) const
	{
		list<string> result;

		int i = 0;
		uint modId = 0;
		while( ( modId = g_modifiers[i++] ) != 0 )
		{
			if( modifiers & modId )
			{
				string modSym = getModifierSymbol( modId );
				if( modSym.empty() == false )
				{
					result.push_back( modSym );
				}
			}
		}

		return result;
	}

	uint KeyMap::getModifiersIds( const std::list<std::string> &lstModif ) const
	{
		uint modifiers = 0;

		list<string>::const_iterator it = lstModif.begin();
		while( it != lstModif.end() )
		{
			string modSym = *it++;
			uint modId = getModifierId( modSym );
			if( modId )
			{
				modifiers |= modId;
			}
		}

		return modifiers;
	}


    //	
    // relative axes
    // 
    
    list<string> KeyMap::getRelAxesSymbols() const
    {
        list<string> syms;
                
        map<uint, string>::const_iterator it = d->relSyms.begin();
        while( it != d->relSyms.end() )
        {
            syms.push_back( (*it++).second );
        }
        
        return syms;
    }
    
    string KeyMap::getRelAxisSymbol( uint axisId ) const
    {
        string axisSym;
		
		map<uint, string>::const_iterator it = d->relSyms.find( axisId );
		if( it != d->relSyms.end() )
		{
			axisSym = (*it).second;
		}
		else
			JSMAPPER_LOG_WARNING( "Unrecognized axis ID '0x%x'", axisId );
		
		return axisSym;
    }

    uint KeyMap::getRelAxisId( const std::string &axisSym ) const
    {
        uint axisId = 0;
		
		map<string, uint>::const_iterator it = d->relIds.find( axisSym );
		if( it != d->relIds.end() )
		{
			axisId = (*it).second;
		}
		else
			JSMAPPER_LOG_WARNING( "Unrecognized axis symbol '%s'", axisSym.c_str() );
		
		return axisId;
    }
    
    
    //
    // Initialization
    //
    
	// Helper macro designed to make key symbol mapping easier, i.e.:
	//		ADD_KEY( A ); ==> addKey( KEY_A, "A" );
	#define ADD_KEY( _ID_ ) 	addKey( KEY_##_ID_, #_ID_)
    #define ADD_BTN( _ID_ ) 	addButton( BTN_##_ID_, #_ID_)
    #define ADD_REL( _ID_ ) 	addRel( REL_##_ID_, #_ID_)
	
	void KeyMap::init()
	{
		// keys:
		ADD_KEY( ESC );
		ADD_KEY( 1 );
		ADD_KEY( 2 );
		ADD_KEY( 3 );
		ADD_KEY( 4 );
		ADD_KEY( 5 );
		ADD_KEY( 6 );
		ADD_KEY( 7 );
		ADD_KEY( 8 );
		ADD_KEY( 9 );
		ADD_KEY( 0 );
		ADD_KEY( MINUS );
		ADD_KEY( EQUAL );
		ADD_KEY( BACKSPACE );
		ADD_KEY( TAB );
		ADD_KEY( Q );
		ADD_KEY( W );
		ADD_KEY( E );
		ADD_KEY( R );
		ADD_KEY( T );
		ADD_KEY( Y );
		ADD_KEY( U );
		ADD_KEY( I );
		ADD_KEY( O );
		ADD_KEY( P );
		ADD_KEY( LEFTBRACE );
		ADD_KEY( RIGHTBRACE );
		ADD_KEY( ENTER );
		ADD_KEY( LEFTCTRL );
		ADD_KEY( A );
		ADD_KEY( S );
		ADD_KEY( D );
		ADD_KEY( F );
		ADD_KEY( G );
		ADD_KEY( H );
		ADD_KEY( J );
		ADD_KEY( K );
		ADD_KEY( L );
		ADD_KEY( SEMICOLON );
		ADD_KEY( APOSTROPHE );
		ADD_KEY( GRAVE );
		ADD_KEY( LEFTSHIFT );
		ADD_KEY( BACKSLASH );
		ADD_KEY( Z );
		ADD_KEY( X );
		ADD_KEY( C );
		ADD_KEY( V );
		ADD_KEY( B );
		ADD_KEY( N );
		ADD_KEY( M );
		ADD_KEY( COMMA );
		ADD_KEY( DOT );
		ADD_KEY( SLASH );
		ADD_KEY( RIGHTSHIFT );
		ADD_KEY( KPASTERISK );
		ADD_KEY( LEFTALT );
		ADD_KEY( SPACE );
		ADD_KEY( CAPSLOCK );
		ADD_KEY( F1 );
		ADD_KEY( F2 );
		ADD_KEY( F3 );
		ADD_KEY( F4 );
		ADD_KEY( F5 );
		ADD_KEY( F6 );
		ADD_KEY( F7 );
		ADD_KEY( F8 );
		ADD_KEY( F9 );
		ADD_KEY( F10 );
		ADD_KEY( NUMLOCK );
		ADD_KEY( SCROLLLOCK );
		ADD_KEY( KP7 );
		ADD_KEY( KP8 );
		ADD_KEY( KP9 );
		ADD_KEY( KPMINUS );
		ADD_KEY( KP4 );
		ADD_KEY( KP5 );
		ADD_KEY( KP6 );
		ADD_KEY( KPPLUS );
		ADD_KEY( KP1 );
		ADD_KEY( KP2 );
		ADD_KEY( KP3 );
		ADD_KEY( KP0 );
		ADD_KEY( KPDOT );
		ADD_KEY( ZENKAKUHANKAKU );
		ADD_KEY( F11 );
		ADD_KEY( F12 );
		ADD_KEY( RO );
		ADD_KEY( KATAKANA );
		ADD_KEY( HIRAGANA );
		ADD_KEY( HENKAN );
		ADD_KEY( KATAKANAHIRAGANA );
		ADD_KEY( MUHENKAN );
		ADD_KEY( KPJPCOMMA );
		ADD_KEY( KPENTER );
		ADD_KEY( RIGHTCTRL );
		ADD_KEY( KPSLASH );
		ADD_KEY( SYSRQ );
		ADD_KEY( RIGHTALT );
		ADD_KEY( LINEFEED );
		ADD_KEY( HOME );
		ADD_KEY( UP );
		ADD_KEY( PAGEUP );
		ADD_KEY( LEFT );
		ADD_KEY( RIGHT );
		ADD_KEY( END );
		ADD_KEY( DOWN );
		ADD_KEY( PAGEDOWN );
		ADD_KEY( INSERT );
		ADD_KEY( DELETE );
		ADD_KEY( MACRO );
		ADD_KEY( MUTE );
		ADD_KEY( VOLUMEDOWN );
		ADD_KEY( VOLUMEUP );
		ADD_KEY( POWER );
		ADD_KEY( KPEQUAL );
		ADD_KEY( KPPLUSMINUS );
		ADD_KEY( PAUSE );
		ADD_KEY( SCALE );
		ADD_KEY( KPCOMMA );
		ADD_KEY( HANGEUL );
		ADD_KEY( HANJA );
		ADD_KEY( YEN );
		ADD_KEY( LEFTMETA );
		ADD_KEY( RIGHTMETA );
		ADD_KEY( COMPOSE );
		ADD_KEY( STOP );
		ADD_KEY( AGAIN );
		ADD_KEY( PROPS );
		ADD_KEY( UNDO );
		ADD_KEY( FRONT );
		ADD_KEY( COPY );
		ADD_KEY( OPEN );
		ADD_KEY( PASTE );
		ADD_KEY( FIND );
		ADD_KEY( CUT );
		ADD_KEY( HELP );
		ADD_KEY( MENU );
		ADD_KEY( CALC );
		ADD_KEY( SETUP );
		ADD_KEY( SLEEP );
		ADD_KEY( WAKEUP );
		ADD_KEY( FILE );
		ADD_KEY( SENDFILE );
		ADD_KEY( DELETEFILE );
		ADD_KEY( XFER );
		ADD_KEY( PROG1 );
		ADD_KEY( PROG2 );
		ADD_KEY( WWW );
		ADD_KEY( MSDOS );
		ADD_KEY( COFFEE );
		ADD_KEY( SCREENLOCK );
		ADD_KEY( DIRECTION );
		ADD_KEY( CYCLEWINDOWS );
		ADD_KEY( MAIL );
		ADD_KEY( BOOKMARKS );
		ADD_KEY( COMPUTER );
		ADD_KEY( BACK );
		ADD_KEY( FORWARD );
		ADD_KEY( CLOSECD );
		ADD_KEY( EJECTCD );
		ADD_KEY( EJECTCLOSECD );
		ADD_KEY( NEXTSONG );
		ADD_KEY( PLAYPAUSE );
		ADD_KEY( PREVIOUSSONG );
		ADD_KEY( STOPCD );
		ADD_KEY( RECORD );
		ADD_KEY( REWIND );
		ADD_KEY( PHONE );
		ADD_KEY( ISO );
		ADD_KEY( CONFIG );
		ADD_KEY( HOMEPAGE );
		ADD_KEY( REFRESH );
		ADD_KEY( EXIT );
		ADD_KEY( MOVE );
		ADD_KEY( EDIT );
		ADD_KEY( SCROLLUP );
		ADD_KEY( SCROLLDOWN );
		ADD_KEY( KPLEFTPAREN );
		ADD_KEY( KPRIGHTPAREN );
		ADD_KEY( NEW );
		ADD_KEY( REDO );
		ADD_KEY( F13 );
		ADD_KEY( F14 );
		ADD_KEY( F15 );
		ADD_KEY( F16 );
		ADD_KEY( F17 );
		ADD_KEY( F18 );
		ADD_KEY( F19 );
		ADD_KEY( F20 );
		ADD_KEY( F21 );
		ADD_KEY( F22 );
		ADD_KEY( F23 );
		ADD_KEY( F24 );
		ADD_KEY( PLAYCD );
		ADD_KEY( PAUSECD );
		ADD_KEY( PROG3 );
		ADD_KEY( PROG4 );
		ADD_KEY( DASHBOARD );
		ADD_KEY( SUSPEND );
		ADD_KEY( CLOSE );
		ADD_KEY( PLAY );
		ADD_KEY( FASTFORWARD );
		ADD_KEY( BASSBOOST );
		ADD_KEY( PRINT );
		ADD_KEY( HP );
		ADD_KEY( CAMERA );
		ADD_KEY( SOUND );
		ADD_KEY( QUESTION );
		ADD_KEY( EMAIL );
		ADD_KEY( CHAT );
		ADD_KEY( SEARCH );
		ADD_KEY( CONNECT );
		ADD_KEY( FINANCE );
		ADD_KEY( SPORT );
		ADD_KEY( SHOP );
		ADD_KEY( ALTERASE );
		ADD_KEY( CANCEL );
		ADD_KEY( BRIGHTNESSDOWN );
		ADD_KEY( BRIGHTNESSUP );
		ADD_KEY( MEDIA );
		ADD_KEY( SWITCHVIDEOMODE );
		ADD_KEY( KBDILLUMTOGGLE );
		ADD_KEY( KBDILLUMDOWN );
		ADD_KEY( KBDILLUMUP );
		ADD_KEY( SEND );
		ADD_KEY( REPLY );
		ADD_KEY( FORWARDMAIL );
		ADD_KEY( SAVE );
		ADD_KEY( DOCUMENTS );
		ADD_KEY( BATTERY );
		ADD_KEY( BLUETOOTH );
		ADD_KEY( WLAN );
		ADD_KEY( UWB );
		ADD_KEY( VIDEO_NEXT );
		ADD_KEY( VIDEO_PREV );
		ADD_KEY( BRIGHTNESS_CYCLE );
		ADD_KEY( BRIGHTNESS_ZERO );
		ADD_KEY( DISPLAY_OFF );
		ADD_KEY( WIMAX );
		ADD_KEY( RFKILL );
	
        // buttons (only mouse buttons, for now)
        ADD_BTN( LEFT );
        ADD_BTN( RIGHT );
        ADD_BTN( MIDDLE );
        
		// modifiers:
		addModifier( JSMAPPER_MODIFIER_SHIFT_L, JSMAPPER_XML_MODIFIER_SHIFT_L );
		addModifier( JSMAPPER_MODIFIER_SHIFT_R, JSMAPPER_XML_MODIFIER_SHIFT_R );
		addModifier( JSMAPPER_MODIFIER_CTRL_L, JSMAPPER_XML_MODIFIER_CTRL_L );
		addModifier( JSMAPPER_MODIFIER_CTRL_R, JSMAPPER_XML_MODIFIER_CTRL_R );
		addModifier( JSMAPPER_MODIFIER_ALT_L, JSMAPPER_XML_MODIFIER_ALT_L );
		addModifier( JSMAPPER_MODIFIER_ALT_R, JSMAPPER_XML_MODIFIER_ALT_R );
		addModifier( JSMAPPER_MODIFIER_META_L, JSMAPPER_XML_MODIFIER_META_L );
		addModifier( JSMAPPER_MODIFIER_META_R, JSMAPPER_XML_MODIFIER_META_R );
        
        // relative axes:
        ADD_REL( X );
        ADD_REL( Y );
        ADD_REL( Z );
        ADD_REL( RX );
        ADD_REL( RY );
        ADD_REL( RZ );
        ADD_REL( HWHEEL );
        ADD_REL( DIAL );
        ADD_REL( WHEEL );
        ADD_REL( MISC );
	}
	
	
	void KeyMap::addKey( uint keyId, const char * keySym )
	{
		d->keySyms[ keyId ] = keySym;
		d->keyIds[ keySym ] = keyId;
	}
	
    void KeyMap::addButton( uint btnId, const char * btnSym )
	{
		d->btnSyms[ btnId ] = btnSym;
		d->btnIds[ btnSym ] = btnId;
	}
	
	void KeyMap::addModifier( uint modId, const char * modSym )
	{
		d->modSyms[ modId ] = modSym;
		d->modIds[ modSym ] = modId;
	}
    
    void KeyMap::addRel( uint relId, const char * relSym )
	{
		d->relSyms[ relId ] = relSym;
		d->relIds[ relSym ] = relId;
	}
}
