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
 * \file monitorclient.h
 * \brief jsmapper::Monitor client for JSMapper KDE profile chooser
 * \author Eduard Huguet <eduardhc@gmail.com>
 */

#ifndef MONITORCLIENT_H
#define MONITORCLIENT_H

#include <jsmapper/monitor.h>
#include <QEvent>

class MonitorClient: public jsmapper::Monitor::Client
{
public:
	MonitorClient( QObject * target );

public:
	virtual void onEvent( jsmapper::Monitor::Event * event );

protected:
	QObject * m_target;
};


/**
 * @brief Event sent by monitor client whenever a device is plugged / unplugged
 */
class MonitorEvent: public QEvent
{
public:
	MonitorEvent( const jsmapper::Monitor::Event * event );

	/**
	 * @brief Returns device event causing the event
	 * @return
	 */
	int getId() const;

	/**
	 * @brief True if the device is being added, false if removed
	 * @return
	 */
	bool isAdded() const;

protected:
	/// Device ID
	int m_id;
	/// True if device is being added, false if removed
	bool m_added;
};



#endif // MONITORCLIENT_H
