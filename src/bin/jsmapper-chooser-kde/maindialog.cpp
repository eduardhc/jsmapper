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
 * \file maindialog.cpp
 * \brief Main dialog class for JSMapper profile chooser app
 * \author Eduard Huguet <eduardhc@gmail.com>
 */

#include "maindialog.h"
#include "model.h"
#include "itemdelegate.h"
#include "actions.h"
#include "settings.h"
#include "systray.h"
#include "monitorclient.h"

#include "ui_maindialog.h"

#include <jsmapper/device.h>
#include <jsmapper/devicemap.h>
#include <jsmapper/profile.h>
#include <jsmapper/monitor.h>

#include <QMessageBox>
#include <QCloseEvent>

#include <KApplication>
#include <KFileDialog>


/**
 * @brief The MainDialog::Private class
 */
class MainDialog::Private
{
public:
	Model				* model;
	SysTray				* tray;
	QList<QAction *>	  trayActions;
	jsmapper::Monitor   * monitor;
	MonitorClient		* monitorClient;

public:
	Private()
	{
		model = NULL;
		tray = NULL;
		monitor = NULL;
		monitorClient = NULL;
	}
};


//

MainDialog::MainDialog( QWidget *parent )
	: QDialog( parent ),
    ui( new Ui::MainDialog ),
	d( new Private() )
{
	ui->setupUi( this );

	d->model = new Model( this );
	ui->lstDevices->setModel( d->model );
	ui->lstDevices->setItemDelegateForColumn( Model::COL_PROFILE,
												new ItemDelegate( this, d->model ) );
	ui->lstDevices->setEditTriggers( QAbstractItemView::SelectedClicked
										| QAbstractItemView::EditKeyPressed );

	connect( ui->lstDevices->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),
			 this, SLOT(currentRowChanged(QModelIndex,QModelIndex)) );
	connect( ui->lstDevices, SIGNAL(doubleClicked(QModelIndex)),
			 this, SLOT(itemDoubleClicked(QModelIndex)) );

	connect( ui->btnLoad, SIGNAL(clicked()), this, SLOT(load()) );
	connect( ui->btnClear, SIGNAL(clicked()), this, SLOT(clear()) );
	connect( ui->btnRefresh, SIGNAL(clicked()), this, SLOT(refresh()) );
	connect( ui->btnExit, SIGNAL(clicked()), this, SLOT(exit()) );

	initTray();
	initMonitor();
	loadSettings();

	KApplication * app = KApplication::kApplication();
	connect( app, SIGNAL(aboutToQuit()), SLOT(finalise()) );

	// load contents:
	refresh();
}



MainDialog::~MainDialog()
{
	delete d;
	delete ui;
}

void MainDialog::finalise()
{
	saveSettings();
	doneTray();
	doneMonitor();
}

void MainDialog::exit()
{
	KApplication * app = KApplication::kApplication();
	app->setQuitOnLastWindowClosed( true );
	
	reject();
}


void /*virtual*/ MainDialog::closeEvent(QCloseEvent * event)
{
	// hide to status bar, instead of closing:
	if( d->tray )
	{
		hide();
	}
	else
	{
		reject();
	}

	event->ignore();
}

//

void MainDialog::currentRowChanged( const QModelIndex &cur, const QModelIndex &prev )
{
	Q_UNUSED(cur);
	Q_UNUSED(prev);
	updateButtons();

	/*
	if( prev.row() >= 0 )
	{
		ui->lstDevices->closePersistentEditor( d->model->index( prev.row(), Model::COL_PROFILE ) );
	}

	if( cur.row() >= 0 )
	{
		ui->lstDevices->openPersistentEditor( d->model->index( cur.row(), Model::COL_PROFILE ) );
	}
	*/
}


//
// load a profile
//

void MainDialog::itemDoubleClicked( const QModelIndex &index )
{
	load( index );
}

void MainDialog::load()
{
	const QModelIndex &index = ui->lstDevices->currentIndex();
	if( index.isValid() )
	{
		load( index );
	}
}

void MainDialog::load( const QModelIndex &index )
{
	Model::Item * item = d->model->getItem( index );
	if( item != NULL )
	{
		loadProfile( item->id );
		d->model->refreshItem( item->id );

		updateButtons();
		updateTray();
	}
}

void MainDialog::loadActionTriggered()
{
	LoadProfileAction * action = dynamic_cast<LoadProfileAction *>( sender() );
	if( action)
	{
		int id = action->getId();
		QString file = action->getFile();
		if( file.isEmpty() )
		{
			loadProfile( id );
		}
		else
		{
			loadProfile( id, file );
		}

		d->model->refreshItem( action->getId() );

		updateTray();
		updateButtons();
	}
}

void MainDialog::loadProfile( int id )
{
	Settings * settings = Settings::getInstance();
	QString lastDir = settings->value( Settings::LAST_OPEN_DIR ).toString();

	QString file = KFileDialog::getOpenFileName(lastDir,
												tr( "*.xml|Profile files (*.xml)" ),
												this,
												tr("Choose profile...") );
	if( file.isEmpty() == false )
	{
		QFileInfo fi( file );
		settings->setValue( Settings::LAST_OPEN_DIR, fi.absolutePath() );

		loadProfile( id, file );
	}
}


//

bool MainDialog::loadProfile( int id, const QString &file )
{
	bool error  = false;

	
	// i.- load the profile:
	jsmapper::Profile profile;
	if( profile.load( file.toLocal8Bit().data() ) == false )
	{
		QMessageBox::critical( this, tr("Error"), tr("Failed to load profile file: '%1'").arg( file ) );
		error = true;
	}
	
	
	// ii.- open device:
	bool opened = false;
	jsmapper::Device dev( id );
	if( error == false )
	{
		if( dev.open() )
		{
			// ok, store profile into the LRU list, even if any succesive step fails
			QString devName = QString::fromLocal8Bit( dev.getName().c_str() );
			QString profileName = QString::fromLocal8Bit( profile.getName().c_str() );
			Settings::getInstance()->addLRUProfile( devName, profileName, file );

			opened = true;
		}
		else
		{
			QMessageBox::critical( this, tr("Error"),
								   tr("Failed to open device: '%1'")
									.arg( QString::fromLocal8Bit( dev.getPath().c_str() ) ) );
			error = true;
		}
	}


	// iii.- find & load map file:
	if( error == false )
	{
		jsmapper::DeviceMap * map = new jsmapper::DeviceMap( &dev );
		
		std::string mapFile = jsmapper::DeviceMap::find( &dev );
		if( mapFile.empty() )
		{
			if( QMessageBox::warning( this, 
										tr("Map file"), 
										tr("No device map file found for device '%1'. Continue?")
											.arg( QString::fromLocal8Bit( dev.getName().c_str() )),
										QMessageBox::Ok | QMessageBox::Cancel, 
										QMessageBox::Cancel ) != QMessageBox::Ok )
			{
				error = true;
			}
	
		}
		else
		{
			if( map->load( mapFile ) == false )
			{
				QMessageBox::critical( this, tr("Error"), tr("Failed to load device map file: '%1'")
										.arg( QString::fromLocal8Bit( mapFile.c_str() ) ) );
				error = true;
			}
		}
		
		dev.setDeviceMap( map );
	}
			

	// iv.- finally, load profile into device:
	if( error == false )
	{
		if( profile.toDevice( &dev ) )
		{
			QString msg = tr("Profile loaded: %1")
							.arg( QString::fromLocal8Bit( profile.getName().c_str() ) );
			notifyTray( msg, QString::fromLocal8Bit( dev.getName().c_str() ) );
		}
		else
		{
			QMessageBox::critical( this, tr("Error"),
								   tr("Failed to load profile into device!") );
			error = true;
		}
	}

	
	// v.- cleanup:
	if( opened )
		dev.close();

	return !error;
}


//
// clear profile
//

void MainDialog::clear()
{
	const QModelIndex &index = ui->lstDevices->currentIndex();

	Model::Item * item = d->model->getItem( index );
	if( item != NULL )
	{
		clearProfile( item->id );
		d->model->refreshItem( item->id );

		updateButtons();
		updateTray();
	}
}

void MainDialog::clearActionTriggered()
{
	ClearProfileAction * action = dynamic_cast<ClearProfileAction *>( sender() );
	if( action)
	{
		if( clearProfile( action->getId() ) )
		{
			d->model->refreshItem( action->getId() );

			updateTray();
			updateButtons();
		}
	}
}

bool MainDialog::clearProfile( int id )
{
	bool error  = false;

	
	// i.- open device:
	bool opened = false;
	jsmapper::Device dev( id );
	if( error == false )
	{
		if( dev.open() )
		{
			opened = true;
		}
		else
		{
			QMessageBox::critical( this, tr("Error"),
								   tr("Failed to open device: '%1'")
									.arg( QString::fromLocal8Bit( dev.getPath().c_str() ) ) );
			error = true;
		}
	}
	

	// ii.- clear device:	
	if( error == false )
	{
		if( dev.clear() )
		{
			notifyTray( tr("Profile cleared"),
						QString::fromLocal8Bit( dev.getName().c_str() ) );
		}
		else
		{
			QMessageBox::critical( this, tr("Error"), tr("Failed to clear device!") );
			error = true;
		}
	}

	
	// iii.- cleanup:
	if( opened )
		dev.close();

	return !error;
}

//

void MainDialog::refresh()
{
	d->model->refresh();

	updateTray();
	updateButtons();
}

//

void MainDialog::updateButtons()
{
	QModelIndex index = ui->lstDevices->currentIndex();

	Model::Item * item = NULL;
	if( index.isValid()
		&& (item = d->model->getItem( index )) != NULL )
	{
		ui->btnLoad->setEnabled( true );
		ui->btnClear->setEnabled( item->profile.isEmpty() == false );
	}
	else
	{
		ui->btnLoad->setEnabled( false );
		ui->btnClear->setEnabled( false );
	}
}


//
// tray icon handling
//

void MainDialog::initTray()
{
	d->tray = new SysTray( this, d->model );
}

void MainDialog::doneTray()
{
	if( d->tray )
	{
		delete d->tray;
		d->tray = NULL;
	}
}

void MainDialog::updateTray()
{
	if( d->tray )
	{
		d->tray->update();
	}
}

void MainDialog::notifyTray( const QString &msg, const QString &device /*= QString()*/ )
{
	if( d->tray )
	{
		QString text = "<center><br>";
		if( device.isEmpty() == false )
		{
			text += QString( "<b>%1</b><br>").arg( device );
		}
		text += msg;

		d->tray->notify( text );
	}
}


//

bool /*virtual*/ MainDialog::event ( QEvent * e )
{
	bool result = false;

	if( e->type() == QEvent::User )
	{
		MonitorEvent * monEv = dynamic_cast<MonitorEvent *>( e );
		if( monEv )
		{
			refresh();

			// notify through systray:
			QString msg, device;
			if( monEv->isAdded() )
			{
				jsmapper::Device dev( monEv->getId() );
				device = QString::fromLocal8Bit( dev.getName().c_str() );
				msg = tr( "Device connected" );
			}
			else
			{
				msg = tr( "Device removed" );
			}
			notifyTray( msg, device );

			monEv->accept();
			result = true;
		}
	}

	if( result == false )
		result = QDialog::event( e );

	return result;
}


//
// Device monitoring
//

void MainDialog::initMonitor()
{
	d->monitor = new jsmapper::Monitor();

	d->monitorClient = new MonitorClient( this );
	d->monitor->addClient( d->monitorClient );

	d->monitor->start();
}

void MainDialog::doneMonitor()
{
	if( d->monitor )
	{
		d->monitor->stop();

		d->monitor->removeClient( d->monitorClient );
		delete d->monitorClient;
		d->monitorClient = NULL;

		delete d->monitor;
		d->monitor = NULL;
	}
}


//
// load / save settings
//

void MainDialog::loadSettings()
{
	Settings * settings = Settings::getInstance();
	settings->beginGroup( Settings::MAIN_DLG_GROUP );

		resize( settings->value( Settings::SIZE, QSize(455, 200)).toSize() );
		move( settings->value( Settings::POS, QPoint(200, 200)).toPoint() );

		for( int i = 0; i < d->model->columnCount(); i++ )
		{
			int colW = ui->lstDevices->columnWidth( i );
			colW = settings->value( Settings::COLUMN_WIDTH_I.arg(i), colW ).toInt();
			ui->lstDevices->setColumnWidth( i, colW );
		}

	settings->endGroup();
}

void MainDialog::saveSettings()
{
	Settings * settings = Settings::getInstance();
	settings->beginGroup( Settings::MAIN_DLG_GROUP );

		settings->setValue( Settings::SIZE, size() );
		settings->setValue( Settings::POS, pos() );

		for( int i = 0; i < d->model->columnCount(); i++ )
		{
			int colW = ui->lstDevices->columnWidth( i );
			settings->setValue( Settings::COLUMN_WIDTH_I.arg(i), colW );
		}

	settings->endGroup();
 }
