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
 * \file actions.h
 * \brief Action subclasses for JSMapper profile chooser app
 * \author Eduard Huguet <eduardhc@gmail.com>
 */

#ifndef ACTIONS_H
#define ACTIONS_H

#include <kaction.h>

/**
 * @brief Base class for device-related actions
 *
 * Provides device context info for subclasses
 */
class DeviceAction : public KAction
{
	Q_OBJECT
protected:
	/**
	 * @brief Protected constructor
	 * @param text
	 * @param deviceId
	 * @param parent
	 */
	DeviceAction( const QString &text, int deviceId, QObject *parent = 0 );
	
public:
	/** Returns device ID */
	int getId() const;

protected:
	/** Device ID */
	int m_deviceId;
};


/**
 * @brief Action used to trigger a profile load
 *
 * If no profile file is given, then this action will be used for the "Load..."
 * menu entry
 */
class LoadProfileAction: public DeviceAction
{
public:
	LoadProfileAction( const QString &text, int deviceId,
					   const QString &file = QString(),
					   QObject *parent = 0 );

public:
	/** Returns profile file (empty for "Load..." action) */
	const QString &getFile() const;

protected:
	/** Profile file */
	QString m_file;
};


/**
 * @brief Action used to trigger a profile clean for a device
 */

class ClearProfileAction: public DeviceAction
{
public:
	/**
	 * @brief ClearProfileAction
	 * @param text
	 * @param deviceId
	 * @param parent
	 */
	ClearProfileAction( const QString &text, int deviceId, QObject *parent = 0 );
};


#endif // ACTIONS_H
