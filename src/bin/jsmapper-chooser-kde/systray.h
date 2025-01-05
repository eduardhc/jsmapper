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
 * \file systray.h
 * \brief System tray handler class declaration
 * \author Eduard Huguet <eduardhc@gmail.com>
 */

#ifndef SYSTRAY_H
#define SYSTRAY_H

#include <kstatusnotifieritem.h>

class MainDialog;
class Model;

/**
 * @brief System tray helper for JSMapper KDE chooser
 */
class SysTray : public KStatusNotifierItem
{
	Q_OBJECT

public:
	/** Tray icon title */
	static const char * TRAY_TITLE;

public:
	SysTray( MainDialog * parent, Model * model );
	virtual ~SysTray();

public:
	MainDialog * parent() const;

public slots:
	/**
	 * @brief Updates tray icon according to model status
	 */
	void update();

	/**
	 * @brief Notifies a message through system tray
	 * @param msg
	 */
	void notify( const QString &msg	);

private:
	class Private;
	Private * d;
};

#endif // SYSTRAY_H
