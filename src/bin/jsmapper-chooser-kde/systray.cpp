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
 * \file systray.cpp
 * \brief System tray handler class implementation
 * \author Eduard Huguet <eduardhc@gmail.com>
 */

#include "main.h"

#include "systray.h"
#include "maindialog.h"
#include "model.h"
#include "actions.h"
#include "settings.h"

#include <KMenu>
#include <QFileInfo>

/**
 * @brief The SysTray::Private class
 */

class SysTray::Private
{
public:
	Model				* model;
	QList<QAction *>	trayActions;

public:
	Private()
		: model( NULL )
	{
	}
};


const char * /*static*/ SysTray::TRAY_TITLE = "JSMapper";

//

SysTray::SysTray( MainDialog * parent, Model * model )
	: KStatusNotifierItem( parent )
{
	d = new Private();
	d->model = model;

	setTitle( TRAY_TITLE );
	setIconByName( APP_ICON );
	setStatus( KStatusNotifierItem::Passive );
	setCategory( KStatusNotifierItem::ApplicationStatus );
}

SysTray::~SysTray()
{
	delete d;
}

MainDialog * SysTray::parent() const
{
	return static_cast<MainDialog *>( KStatusNotifierItem::parent() );
}


void SysTray::notify( const QString &msg )
{
	// TODO use KNotification
	showMessage( TRAY_TITLE, msg, APP_ICON );
}


void SysTray::update()
{
	MainDialog * dlg = parent();

	// clear old actions:
	KMenu * menu = contextMenu();
	foreach( QAction * action, d->trayActions )
	{
		menu->removeAction( action );
		delete action;
	}
	d->trayActions.clear();


	// actions will be inserted after title action (which is actions[0]),
	// but before standard actions such as minimize/restore & exit
	QAction * beforeAction = NULL;
	QList<QAction *> actions = menu->actions();
	if( actions.size() > 1 )
		beforeAction = actions[1];


	// add device actions:
	ItemStatus status = KStatusNotifierItem::Passive;
	QString tooltip = "<center><br>";

	int deviceCount = d->model->rowCount();
	if( deviceCount > 0 )
	{
		status = KStatusNotifierItem::Active;

		Settings * settings = Settings::getInstance();
		for( int i = 0; i < deviceCount; i++ )
		{
			Model::Item * item = d->model->getItem( i );

			tooltip += QString( "<b>%1</b><br>" ).arg( item->name );

			// build menu:
			KMenu * deviceMenu = new KMenu( dlg );
			KAction * action = new LoadProfileAction( tr("&Load Profile..."),
															item->id,
															QString(),
															this );
			connect( action, SIGNAL(triggered()), dlg, SLOT(loadActionTriggered()) );
			deviceMenu->addAction( action );
			d->trayActions.push_back( action );

			// add LRU list:
			Settings::LRUProfileList profiles = settings->loadLRUProfileList( item->name );
			if( profiles.size() > 0 )
			{
				deviceMenu->addSeparator();

				Settings::LRUProfileList::const_iterator it = profiles.begin();
				while( it != profiles.end() )
				{
					Settings::LRUProfile profile = *it++;

					QFileInfo fi( profile.file );
					KAction * action = new LoadProfileAction( QString( "%0 (%1)" )
																	.arg( profile.name )
																	.arg( fi.fileName() ),
																item->id,profile.file,
																this );
					connect( action, SIGNAL(triggered()), dlg, SLOT(loadActionTriggered()) );
					deviceMenu->addAction( action );
					d->trayActions.push_back( action );
				}
			}

			if( item->profile.isEmpty() == false )
			{
				tooltip += tr( "Profile: %1" ).arg( item->profile );

				// add "Clear" action:
				deviceMenu->addSeparator();
				action = new ClearProfileAction( tr("&Clear Profile"),
												 item->id,
												 this );
				connect( action, SIGNAL(triggered()), dlg, SLOT(clearActionTriggered()) );
				deviceMenu->addAction( action );
				d->trayActions.push_back( action );
			}
			else
			{
				tooltip += tr( "Profile: (none)" );
			}


			KAction * deviceMenuAction = new KAction( item->name, this );
			deviceMenuAction->setMenu( deviceMenu );
			menu->insertAction( beforeAction, deviceMenuAction );
			d->trayActions.push_back( deviceMenuAction );
		}

	}
	else
	{
		tooltip += tr( "(no devices)" );
	}

	setStatus( status );
	setToolTip( APP_ICON, TRAY_TITLE, tooltip );
}



