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
 * \file monitorclient.cpp
 * \brief jsmapper::Monitor client for JSMapper KDE profile chooser
 * \author Eduard Huguet <eduardhc@gmail.com>
 */

#include "monitorclient.h"

#include <QCoreApplication>

MonitorClient::MonitorClient( QObject * target )
	: m_target( target )
{
}

void /*virtual*/ MonitorClient::onEvent( jsmapper::Monitor::Event * event )
{
	QCoreApplication::postEvent( m_target, new MonitorEvent( event ) );
}

//

MonitorEvent::MonitorEvent( const jsmapper::Monitor::Event * event )
	: QEvent( QEvent::User ),
	  m_id( event->id ),
	  m_added( event->type == jsmapper::Monitor::EVENT_DEVICE_ADDED )
{
}

int MonitorEvent::getId() const
{
	return m_id;
}

bool MonitorEvent::isAdded() const
{
	return m_added;
}

