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

#ifndef __SETTINGS_H
#define __SETTINGS_H

#include <QSettings>

/**
 * @brief Settings class for JSMapper profile chooser app
 *
 * This singleton class takes care of app settings save & restore
 * process.
 */
class Settings: public QSettings
{
public:
	/**
	 * @brief Returns singleton settings object
	 * @return
	 */
	static Settings * getInstance();

	/**
	 * @brief Destroys the singleton settings object
	 */
	static void releaseInstance( Settings * settings );


// value keys:
public:
	/** Main dialog settings group name */
	static QString MAIN_DLG_GROUP;
	/** Main dialog position */
	static QString POS;
	/** Main dialog size */
	static QString SIZE;
	/** Column width */
	static QString COLUMN_WIDTH_I;
	
	/** Last directory used for opening */
	static QString LAST_OPEN_DIR;
	
	/** LRU count for device */
	static QString LRU_S_COUNT;
	/** LRU item name for device */
	static QString LRU_S_ITEM_I_NAME;
	/** LRU item file for device */
	static QString LRU_S_ITEM_I_FILE;
	

// operations
public:
	/**
	 * @brief Helper class representing a LRU profile for a device
	 */
	class LRUProfile
	{
	public:
		/** Profile name */
		QString name;
		/** Profile file */
		QString file;
	};

	typedef QList<LRUProfile> LRUProfileList;

	/**
	 * @brief Adds a profile to the LRU list for a device
	 *
	 * Registers a profile into the LRU (Last Recently Used) list for the given
	 * device. If the profile was yet registered, then it's moved to the top
	 * position on the list.
	 *
	 * @param devName Device name
	 * @param profileName Profile name
	 * @param profileFile Profile file
	 */
	void addLRUProfile( const QString &devName, const QString &profileName,
						const QString &profileFile );


	/**
	 * @brief Returns the LRU profile list for a device
	 * @param devName Device name to query LRU for
	 * @return
	 */
	LRUProfileList loadLRUProfileList( const QString &devName );

	/**
	 * @brief Saves LRU profile list for a device
	 * @param profiles Profile list for the device
	 */
	void saveLRUProfileList(  const QString &devName, const LRUProfileList &profiles );


protected:
	/**
	 * @brief Protected constructor
	 */
	Settings();
	virtual ~Settings();

	/**
	 * @brief The singleton object
	 */
	static Settings * g_settings;


private:
	class Private;
	Private * d;
};

#endif // __SETTINGS_H
