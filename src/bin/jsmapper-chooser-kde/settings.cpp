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
 * \file settings.h
 * \brief Settings class for JSMapper profile chooser app
 * \author Eduard Huguet <eduardhc@gmail.com>
 */

#include "settings.h"
#include "main.h"

#include <algorithm>

QString Settings::MAIN_DLG_GROUP	= "MainDlg";
QString Settings::POS				= "pos";
QString Settings::SIZE				= "size";
QString Settings::LAST_OPEN_DIR		= "LastOpenDir";
QString Settings::COLUMN_WIDTH_I	= "column_width.%1";
QString Settings::LRU_S_COUNT		= "lru.%1/count";
QString Settings::LRU_S_ITEM_I_NAME = "lru.%1/item.%2/name";
QString Settings::LRU_S_ITEM_I_FILE = "lru.%1/item.%2/file";

	
	
/**
 * @brief The Settings::Private class
 */

class Settings::Private
{
public:
	int maxLRUCount;

public:
	Private()
		: maxLRUCount( 10 )
	{
	}
};

//

Settings * /*static*/ Settings::g_settings = NULL;

//

Settings * /*static*/ Settings::getInstance()
{
	if( g_settings == NULL )
	{
		g_settings = new Settings();
		// TODO
	}

	return g_settings;
}

//

void /*static*/ Settings::Settings::releaseInstance( Settings * settings )
{
	if( settings == g_settings )
	{
		delete g_settings;
		g_settings = NULL;
	}
}

//

Settings::Settings()
	:  QSettings( ORGANIZATION_NAME, APP_NAME )
{
	d = new Private();
}

Settings::~Settings()
{
	delete d;
}


//
// Operations
//

void Settings::addLRUProfile( const QString &devName,
								const QString &profileName,
								const QString &profileFile )
{
	// search item in current profile list, and remove it if found:
	LRUProfileList profiles = loadLRUProfileList( devName );
	LRUProfileList::iterator it = profiles.begin();
	while( it != profiles.end() )
	{
		const LRUProfile &item = *it;
		if( item.file == profileFile )
		{
			it = profiles.erase( it );
		}
		else
			++it;
	}

	// insert new item at the beginning of the list:
	LRUProfile item;
	item.name = profileName;
	item.file = profileFile;
	profiles.push_front( item );

	// purge items to keep list in size:
	while( profiles.size() > d->maxLRUCount )
		profiles.takeLast();

	// save new list, finally:
	saveLRUProfileList( devName, profiles );
}

//

Settings::LRUProfileList Settings::loadLRUProfileList( const QString &devName )
{
	LRUProfileList result;

	int count = value( LRU_S_COUNT.arg( devName ) ).toInt();
	count = std::min( count, d->maxLRUCount );
	for( int i = 0; i < count; i++ )
	{
		LRUProfile item;
		item.name = value( LRU_S_ITEM_I_NAME.arg( devName ).arg( i ) ).toString();
		item.file = value( LRU_S_ITEM_I_FILE.arg( devName ).arg( i ) ).toString();
		result.push_back( item );
	}

	return result;
}

//

void Settings::saveLRUProfileList( const QString &devName, const LRUProfileList &profiles )
{
	int count = std::min( profiles.size(), d->maxLRUCount );
	setValue( LRU_S_COUNT.arg( devName ), count );

	int i = 0;
	LRUProfileList::const_iterator it = profiles.begin();
	while( it != profiles.end() )
	{
		const LRUProfile &item = *it++;
		setValue( LRU_S_ITEM_I_NAME.arg( devName ).arg( i ), item.name );
		setValue( LRU_S_ITEM_I_FILE.arg( devName ).arg( i ), item.file );
		i++;
	}
}

