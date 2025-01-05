#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "app.h"
#include "newprofiledlg.h"

#include "actions/actionsview.h"

#include <QDockWidget>
#include <QFileDialog>
#include <QTableView>
#include <QTreeView>
#include <QMessageBox>
#include <QSettings>
#include <QDebug>

/**
 * @brief The MainWindow::Private class
 */
class MainWindow::Private
{
public:
	QTableView		* controlsView;
	ActionsView		* actionsView;
	QString			lastDir;
	
public:
	Private() 
		: controlsView( NULL ), 
		  actionsView( NULL ), 
		  lastDir( QDir::homePath() )
	{
	}
};


//

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	d = new Private();
	
	connect( ui->actionNew, SIGNAL(triggered(bool)), this, SLOT(onNew()));
	connect( ui->actionOpen, SIGNAL(triggered(bool)), this, SLOT(onOpen()));
	connect( ui->actionSave, SIGNAL(triggered(bool)), this, SLOT(onSave()));
	connect( ui->actionSaveAs, SIGNAL(triggered(bool)), this, SLOT(onSaveAs()));
	connect( ui->actionQuit, SIGNAL(triggered(bool)), this, SLOT(onQuit()));
	
	App * app = App::instance();
	connect( app, SIGNAL(profileOpen(jsmapper::Profile*)), this, SLOT(onProfileOpen(jsmapper::Profile*)) );
	connect( app, SIGNAL(profileStatusChanged(jsmapper::Profile*)), this, SLOT(onProfileStatusChanged(jsmapper::Profile*)) );
	connect( app, SIGNAL(profileClosed(jsmapper::Profile*)), this, SLOT(onProfileClosed(jsmapper::Profile*)) );
	
	initViews();
	readSettings();
	
	updateTitle();
}

MainWindow::~MainWindow()
{
	delete d;
	d = NULL;
	
	delete ui;
}


//
// Views setup
//

void MainWindow::initViews()
{
	d->controlsView = new QTableView( this );
	setCentralWidget( d->controlsView );
	
	ui->menuWindow->addAction( ui->mainToolBar->toggleViewAction() );
	
	// actions dock bar:
	d->actionsView = new ActionsView( this );
	QDockWidget * dock = new QDockWidget( tr("Actions"), this );
		dock->setObjectName( "ActionsDock" );
		dock->setAllowedAreas( Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea );
		dock->setWidget( d->actionsView );
	addDockWidget( Qt::RightDockWidgetArea, dock );
	ui->menuWindow->addAction( dock->toggleViewAction() );
}

//

void /*virtual*/ MainWindow::closeEvent( QCloseEvent *event )
{
	writeSettings();
	QMainWindow::closeEvent( event );
}



//
// File Menu handling
//

void MainWindow::onNew()
{
	NewProfileDlg dlg( this );
	int ret = dlg.exec();
	if( ret == QDialog::Accepted ) 
	{
		App::instance()->createProfile( dlg.getName(), 
										dlg.getTarget(), 
										dlg.getDescription() ); 
	}
}

void MainWindow::onOpen()
{
	QString file = QFileDialog::getOpenFileName( this, 
												 tr("Open profile"), 
												 d->lastDir,
												 tr("JSMapper Profile files (*.xml)") );
	if( file.isNull() == false )
	{
		QFileInfo fi( file );
		d->lastDir = fi.absolutePath();
		
		if( tryClose() )
		{
			App::instance()->openProfile( file );
		}
	}
}

bool MainWindow::onSave()
{
	bool ret = false;
	
	App * app = App::instance();
	if( app->currentProfile() != NULL )
	{
		if( app->currentProfileFile().isEmpty() == false )
		{
			ret = app->saveProfile( app->currentProfileFile() );
		}
		else
		{
			ret = onSaveAs();
		}
	}
	
	return ret;
}

bool MainWindow::onSaveAs()
{
	bool ret = false;
	
	App * app = App::instance();
	if( app->currentProfile() != NULL )
	{
		QString file = QFileDialog::getSaveFileName( this, 
													 tr("Save profile"), 
													 d->lastDir,
													 tr("JSMapper Profile files (*.xml)") );
		if( file.isNull() == false )
		{
			QFileInfo fi( file );
			d->lastDir = fi.absolutePath();
			
			ret = App::instance()->saveProfile( file );
		}
	}
	
	return ret;
}

//

bool MainWindow::tryClose()
{
	bool ret = true;
	
	App * app = App::instance();
	if( (app->currentProfile() != NULL) && app->isModified() ) 
	{
		int ret = QMessageBox::warning(this, tr("Warning"),
		                               tr("The profile has been modified.\n"
		                                  "Do you want to save your changes?"),
		                               QMessageBox::Save 
											| QMessageBox::Discard
											| QMessageBox::Cancel,
		                               QMessageBox::Save );
		switch( ret )
		{
		case QMessageBox::Save:
			if( onSave() ) {
				// abort if saving fails or cancelled
				ret = false;
			}
			break;
		case QMessageBox::Discard:
			ret = true;		// continue without saving
			break;
		case QMessageBox::Cancel:
			ret = false;	// abort
			break;
		} 
	}
	
	return ret;
}


//

void MainWindow::onQuit()
{
	qDebug( "QUIT" );    
	
	if( tryClose() )
	{
		close();
	}
}



//
// Profile change slots:
//

void MainWindow::onProfileOpen( jsmapper::Profile * )
{
	qDebug( "PROFILE_OPEN" );
	updateTitle();
}

void MainWindow::onProfileStatusChanged( jsmapper::Profile * )
{
	qDebug( "PROFILE_STATUS_CHANGED" );    
	updateTitle();
}

void MainWindow::onProfileClosed( jsmapper::Profile * )
{
	qDebug( "PROFILE_CLOSED" );    
	updateTitle();
}


//

void MainWindow::updateTitle()
{
	QString title = tr("JSMapper Studio");
	
	App * app = App::instance();
	if( app->currentProfile() != NULL )
	{
		title += " - ";
		
		QString path = app->currentProfileFile();
		if( path.isEmpty() == false )
		{
			title += QString("%1").arg( path );
		}
		else
		{
			// new profile - not yet saved
			title += QString( tr("New profile*") );
		}
		
		if( app->isModified() )
		{
			title += "*";
		}
	}
	
	setWindowTitle( title );
}


//
// Settings:
//

const char * MainWindow::SETTINGS_MAINWND_GROUP				= "mainwnd";
const char * MainWindow::SETTINGS_GEOMETRY					= "geometry";
const char * MainWindow::SETTINGS_STATE						= "state";
const char * MainWindow::SETTINGS_LAST_DIR					= "lastDir";


void MainWindow::writeSettings() const
{
	QSettings settings;	
	
	settings.beginGroup( SETTINGS_MAINWND_GROUP );
		settings.setValue( SETTINGS_GEOMETRY, saveGeometry() );
	    settings.setValue( SETTINGS_STATE, saveState() );
		settings.setValue( SETTINGS_LAST_DIR, d->lastDir );
	settings.endGroup();
}

//

void MainWindow::readSettings()
{
	QSettings settings;	
	
	settings.beginGroup( SETTINGS_MAINWND_GROUP );
		restoreGeometry( settings.value( SETTINGS_GEOMETRY ).toByteArray() );
		restoreState( settings.value( SETTINGS_STATE ).toByteArray() );
		d->lastDir = settings.value( SETTINGS_LAST_DIR ).toString();
	settings.endGroup();
}
