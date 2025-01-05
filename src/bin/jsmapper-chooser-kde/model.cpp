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
 * \file devicelistmodel.cpp
 * \brief Device list model for JSMapper profile chooser app
 * \author Eduard Huguet <eduardhc@gmail.com>
 */

#include "model.h"

#include <jsmapper/device.h>
#include <QVector>


/**
 * @brief The Model::Private class
 */
class Model::Private
{
public:
	Private()
	{
	}

public:
	QVector<Item> items;
};



//

Model::Model(QObject *parent) :
	QAbstractTableModel(parent)
{
	d = new Private();
}

Model::~Model()
{
	delete d;
}


//

int /*virtual*/ Model::rowCount ( const QModelIndex & parent /*= QModelIndex()*/ ) const
{
	Q_UNUSED(parent);
	return d->items.size();
}

//

int /*virtual*/ Model::columnCount ( const QModelIndex & parent /*= QModelIndex()*/ ) const
{
	Q_UNUSED(parent);
	return NUM_COL;
}

//

QVariant /*virtual*/ Model::headerData ( int section,
												   Qt::Orientation orientation,
												   int role /*= Qt::DisplayRole*/ ) const
{
	Q_UNUSED(orientation);

	QVariant result;
	switch( role )
	{
	case Qt::DisplayRole:
		switch( section )
		{
		case COL_DEVICE:
			result = QVariant( tr("Device") );
			break;
		case COL_PROFILE:
			result = QVariant( tr("Profile") );
			break;
		default:
			break;
		}
		break;

	default:
		break;
	}

	return result;
}

//

QVariant /*virtual*/ Model::data(const QModelIndex &index,
										   int role /*= Qt::DisplayRole*/) const
{
	QVariant result;

	switch( role )
	{
	case Qt::DisplayRole:
	{
		const Item * item = getItem( index );
		if( item != NULL )
		{
			switch( index.column() )
			{
			case COL_DEVICE:
				result = item->name;
				break;
			case COL_PROFILE:
				result = item->profile;
				break;
			default:
				break;
			}
		}
		break;
	}

	default:
		break;
	}

	return result;
}

//

Qt::ItemFlags /*virtual*/ Model::flags(const QModelIndex &index) const
{
	Qt::ItemFlags f = QAbstractTableModel::flags( index );
	
	/*
	if( index.column() == COL_PROFILE )
		f |= Qt::ItemIsEditable;
	*/

	return f;
}

//

Model::Item * Model::getItem( const QModelIndex &index ) const
{
	return getItem( index.row() );
}

Model::Item * Model::getItem( int row ) const
{
	Item * item = NULL;

	if( row >= 0 && row < d->items.size() )
	{
		item = &d->items[row];
	}

	return item;
}

//

void Model::refresh()
{
	beginResetModel();

	d->items.clear();
	for( int id = 0; id < 10; id++ )
	{
		if( jsmapper::Device::test( id ) )
		{
			Item item( id );
			updateItem( &item );
			d->items.push_back( item );
		}
	}

	endResetModel();
}


//

void Model::refreshItem( int id )
{
	for( int i = 0; i < d->items.size(); i++ )
	{
		Item &item = d->items[i];
		if( item.id == id )
		{
			updateItem( &item );

			QModelIndex topLeft = index( i, COL_DEVICE );
			QModelIndex bottomRight = index( i, COL_PROFILE );
			emit dataChanged( topLeft, bottomRight );
			break;
		}
	}
}


//

bool Model::updateItem( Item * item )
{
	bool ret = false;

	jsmapper::Device * dev = new jsmapper::Device( item->id );
	if( dev->open() )
	{
		item->name = QString::fromLocal8Bit( dev->getName().c_str() );
		item->profile = QString::fromLocal8Bit( dev->getProfileName().c_str() );

		dev->close();
		ret = true;
	}
	else
		item->name = item->profile = tr("Error!");

	return ret;
}

