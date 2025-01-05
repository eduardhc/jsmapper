#include "app.h"
#include "mainwindow.h"

#include <QMessageBox>
#include <jsmapper/profile.h>


///////////////////////////////////////////////////////////////////////////////
//
// App private data
//
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief The App::Private class
 */
class App::Private
{        
public:
	/// Main window
	MainWindow * mainWnd;
	/// Currently opened profile
	jsmapper::Profile * profile;	
	/// Currently opened profile path
	QString profileFile;	
	/// TRUE si profile has been modified and not yet saved
	bool modified; 
	
public:
	Private() 
	    : mainWnd( NULL ), 
	      profile( NULL ), 
		  modified( false )
	{
	}
};


///////////////////////////////////////////////////////////////////////////////
//
// App construction
//
///////////////////////////////////////////////////////////////////////////////

App::App( int argc, char *argv[] )
    : QApplication( argc, argv )
{
	d = new Private();
}

App::~App()
{
	delete d;
	d = NULL;
}

App * /*static*/ App::instance()
{
	return dynamic_cast<App *>( QApplication::instance() );
}


//


int App::run()
{
	d->mainWnd = new MainWindow();
	d->mainWnd->show();
	return exec();
}


///////////////////////////////////////////////////////////////////////////////
//
// Profile handling
//
///////////////////////////////////////////////////////////////////////////////

jsmapper::Profile * App::currentProfile() const
{
	return d->profile;
}

//

QString App::currentProfileFile() const
{
	return d->profileFile;
}

//

void App::createProfile( const QString &name, 
							const QString &target, 
							const QString &description )
{
	closeProfile();
	
	d->profile = new jsmapper::Profile( target.toStdString() );
	d->profile->setName( name.toStdString() );
	d->profile->setDescription( description.toStdString() );
	d->profileFile.clear();
	d->modified = true;
	
	emit profileOpen( d->profile );
}

//

bool App::openProfile( const QString &path )
{
	bool ret = false;
	
	jsmapper::Profile * profile = new jsmapper::Profile();
	if( profile->load( path.toStdString() ) )
	{
		// ok, set as active profile:
		closeProfile();
		
		d->profile = profile;
		d->profileFile = path;
		d->modified = false;
		emit profileOpen( d->profile );
		ret = true;
		
	} 
	else 
	{
		// notify error:	
		QMessageBox::critical( d->mainWnd, 
		                       tr("Error"), 
		                       tr("Failed to open profile from file \"%1\".").arg( path ) );
	}
	
	return ret;
}


//

bool App::saveProfile( const QString &path )
{
	bool ret = false;
	
	if( d->profile != NULL )
	{
		if( d->profile->save( path.toStdString() ) )
		{
			d->profileFile = path;
			d->modified = false;
			emit profileStatusChanged( d->profile );
			ret = true;
		}
		else
		{
			// notify error:	
			QMessageBox::critical( d->mainWnd, 
			                       tr("Error"), 
			                       tr("Failed to save profile to file \"%1\".").arg( path ) );
		}
	}
	
	return ret;
}


//

void App::closeProfile()
{
	if( d->profile != NULL )
	{
		emit profileClosed( d->profile );
		
		delete d->profile;
		d->profile = NULL;
		d->profileFile.clear();
		d->modified = false;
	}
}


//

bool App::isModified() const
{
	return d->modified;
}

//

void App::setModified( bool set /*= true*/ )
{
	d->modified = set;
	emit profileStatusChanged( d->profile );
}
