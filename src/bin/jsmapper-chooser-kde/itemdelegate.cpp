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
 * \file devicelistitemdelegate.cpp
 * \brief Item delegate for device list items
 * \author Eduard Huguet <eduardhc@gmail.com>
 */

#include "itemdelegate.h"
#include "model.h"
#include "settings.h"
#include "actions.h"
#include "maindialog.h"

#include <QComboBox>
#include <QMap>

class ItemDelegate::Private
{
public:
	Model * model;
	QMap<int, DeviceAction *> actions;

public:
	Private()
		: model( NULL )
	{
	}
};


//


ItemDelegate::ItemDelegate( MainDialog * parent, Model * model )
	: QStyledItemDelegate( parent )
{
	d = new Private();
	d->model = model;
}

ItemDelegate::~ItemDelegate()
{
	delete d;
	d = NULL;
}

MainDialog * ItemDelegate::mainDlg() const
{
	return static_cast<MainDialog *>( QStyledItemDelegate::parent() );
}

//

QWidget * /*virtual*/ ItemDelegate::createEditor( QWidget *parent,
													const QStyleOptionViewItem &option,
													const QModelIndex &index ) const
{
	Q_UNUSED(option);
	QWidget * result = NULL;

	MainDialog * dlg = mainDlg();
	Model::Item * item = d->model->getItem( index );
	if( item )
	{
		switch( index.column() )
		{
		case Model::COL_PROFILE:
		{
			QComboBox *comboBox = new QComboBox( parent );
			comboBox->setEditable( false );
			comboBox->setInsertPolicy( QComboBox::InsertAtBottom );

			// current profile:
			QString name = item->profile;
			if( name.isEmpty() )
				name = tr( "(none)" );
			comboBox->addItem( name );
			comboBox->setCurrentIndex( 0 );

			// add LRU items (except current):
			Settings * settings = Settings::getInstance();
			Settings::LRUProfileList profiles = settings->loadLRUProfileList( item->name );
			foreach( const Settings::LRUProfile &profile, profiles )
			{
				if( profile.name != item->profile )
				{
					DeviceAction * action = new LoadProfileAction( profile.name,
																   item->id,
																   profile.file,
																   dlg );
					connect( action, SIGNAL(triggered()),
							 dlg, SLOT( loadActionTriggered() ) );

					comboBox->addItem( profile.name );
					d->actions[ comboBox->count() - 1]	= action;
				}
			}

			// add "none" item at end, if a profile is loaded:
			if( item->profile.isEmpty() == false )
			{
				DeviceAction * action = new ClearProfileAction( tr( "Clear" ),
																item->id,
																dlg );
				connect( action, SIGNAL(triggered()),
						 dlg, SLOT( clearActionTriggered() ) );

				comboBox->addItem( tr("(none)") );
				d->actions[ comboBox->count() - 1]	= action;
			}

			connect( comboBox, SIGNAL(currentIndexChanged(int)),
					 this, SLOT(emitCommitData()) );

			/*
			connect(comboBox, SIGNAL(activated(int)), this, SLOT(emitCommitData()));
			QString currentText = index.model()->data(index, Qt::DisplayRole).toString();
			int selectedItem = comboBox->findText(currentText);
			if(selectedItem == -1)
				comboBox->setEditText(index.model()->data(index, Qt::DisplayRole).toString());
			else
				comboBox->setCurrentIndex(selectedItem);
			*/

			result = comboBox;
			break;
		}

		default:
			break;
		}
	}

	return result;
}


//

void /*virtual*/ ItemDelegate::setEditorData ( QWidget * editor,
												const QModelIndex & index ) const
{
	(void) editor;
	(void) index;
}


//

void /*virtual*/ ItemDelegate::setModelData ( QWidget * editor,
												QAbstractItemModel * model,
												const QModelIndex & index ) const
{
	(void) editor;
	(void) model;
	(void) index;
}

