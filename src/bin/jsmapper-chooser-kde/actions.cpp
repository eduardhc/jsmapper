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

#include "actions.h"

DeviceAction::DeviceAction( const QString &text, int deviceId, QObject *parent /*= 0*/ )
	: KAction( text, parent ),
	  m_deviceId( deviceId )
{
}

int DeviceAction::getId() const
{
	return m_deviceId;
}


//

LoadProfileAction::LoadProfileAction( const QString &text, int deviceId,
										const QString &file /*= QString()*/,
										QObject *parent /*= 0*/ )
	: DeviceAction( text, deviceId, parent ),
	  m_file( file )
{
}

const QString &LoadProfileAction::getFile() const
{
	return m_file;
}

//

ClearProfileAction::ClearProfileAction( const QString &text, int deviceId,
										QObject *parent /*= 0*/ )
	: DeviceAction( text, deviceId, parent )
{
}

