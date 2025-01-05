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
 * \file maindialog.h
 * \brief Main dialog class for JSMapper profile chooser app
 * \author Eduard Huguet <eduardhc@gmail.com>
 */

#ifndef MAINDIALOG_H
#define MAINDIALOG_H

#include <QDialog>
#include <QModelIndex>

namespace Ui {
class MainDialog;
}

class MainDialog : public QDialog
{
	Q_OBJECT
	
public:
	explicit MainDialog(QWidget *parent = 0);
	~MainDialog();
	
public slots:
	void currentRowChanged( const QModelIndex &cur, const QModelIndex &prev );
	void itemDoubleClicked( const QModelIndex &index );
	void load();
	void load( const QModelIndex &index );
	void loadActionTriggered();
	void clear();
	void clearActionTriggered();
	void refresh();
	void updateButtons();
	void finalise();
	void exit();

protected:
	void loadProfile( int id );
	bool loadProfile( int id, const QString &file );
	bool clearProfile( int id );


// tray icon handling
protected:
	void initTray();
	void doneTray();
	void notifyTray( const QString &msg, const QString &device = QString() );
	void updateTray();

// device monitoring
protected:
	void initMonitor();
	void doneMonitor();

// settings for dialog
protected:
	void loadSettings();
	void saveSettings();

protected:
	virtual void closeEvent(QCloseEvent * event);
	virtual bool event ( QEvent * event );

private:
	Ui::MainDialog *ui;

	class Private;
	Private * d;
};

#endif // MAINDIALOG_H
