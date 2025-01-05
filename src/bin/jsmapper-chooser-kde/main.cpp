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
 * \brief Main file for JSMapper profile chooser app
 * \author Eduard Huguet <eduardhc@gmail.com>
 */


#include "main.h"
#include "maindialog.h"
#include "settings.h"

#include <kuniqueapplication.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>

const char * ORGANIZATION_NAME	= "JSMapper";
const char * APP_NAME			= "jsmapper-chooser-kde";
const char * APP_ICON			= "jsmapper";
const char * APP_VERSION		= "1.0.0";

int main(int argc, char *argv[])
{
	KAboutData about( APP_NAME,
					  NULL,
					  ki18n( "JSMapper KDE Profile Chooser" ),
					  APP_VERSION,
                      ki18n( "JSMapper profile chooser for KDE." ),
                      KAboutData::License_GPL_V2,
                      ki18n( "Copyright (C) 2013 Eduard Huguet" ) );
	about.setProgramIconName( APP_ICON );
    about.addAuthor( ki18n("Eduard Huguet"),
					 ki18n("Developer"),
					 "eduardhc@gmail.com", 0 );

	KCmdLineArgs::init( argc, argv, &about );
	KUniqueApplication::addCmdLineOptions();

	if ( !KUniqueApplication::start() )
	{
		exit( 0 );
	}

    KUniqueApplication app;
	app.setQuitOnLastWindowClosed( false );
	
	Settings * settings = Settings::getInstance();

	MainDialog dlg;
	dlg.show();
	int ret = app.exec();

	Settings::releaseInstance( settings );
	return ret;
}
