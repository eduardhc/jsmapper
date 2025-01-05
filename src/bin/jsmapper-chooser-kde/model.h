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
 * \file devicelistmodel.h
 * \brief Device list model for JSMapper profile chooser app
 * \author Eduard Huguet <eduardhc@gmail.com>
 */

#ifndef MODEL_H
#define MODEL_H

#include <QAbstractTableModel>

class Model : public QAbstractTableModel
{
	Q_OBJECT
public:
	explicit Model(QObject *parent = 0);
	virtual ~Model();
	

// data
public:
	/** Column index for device name */
	static const int COL_DEVICE = 0;
	/** Column index for currently loaded profile for device */
	static const int COL_PROFILE = 1;
	/** Number of columns */
	static const int NUM_COL = 2;


	/**
	 * @brief The Model::Item class
	 */
	class Item
	{
	public:
		/** Device ID */
		int id;
		/** Device name */
		QString name;
		/** Currently loaded device profile name */
		QString profile;

	public:
		Item( int id = -1 )
		{
			this->id = id;
		}
	};


	/**
	 * @brief getItem
	 * @param index
	 * @return
	 */
	Item * getItem( const QModelIndex &index ) const;

	/**
	 * @brief getItem
	 * @param row
	 * @return
	 */
	Item * getItem( int row ) const;

// overrides:
public:
	/**
	 * @brief Returns current number of devices detected
	 * @param parent
	 * @return
	 */
	virtual int rowCount ( const QModelIndex & parent = QModelIndex() ) const;

	/**
	 * @brief Returns number of columns to display (always 2)
	 * @param parent
	 * @return
	 */
	virtual int columnCount ( const QModelIndex & parent = QModelIndex() ) const;

	/**
	 * @brief headerData
	 * @param section
	 * @param orientation
	 * @param role
	 * @return
	 */
	virtual QVariant headerData ( int section, Qt::Orientation orientation,
								  int role = Qt::DisplayRole ) const;

	/**
	 * @brief Returns the data to be displayed on the list
	 *
	 * @param index
	 * @param role
	 * @return
	 */
	virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;


	/**
	 * @brief Returns edit flags for item
	 *
	 * @param index
	 * @param role
	 * @return
	 */
	virtual Qt::ItemFlags flags(const QModelIndex &index) const;


signals:
	

public slots:
	/**
	 * @brief Reloads device list
	 */
	void refresh();

	/**
	 * @brief Refreshes status for given item
	 * @param id Item device ID
	 */
	void refreshItem( int id );


protected:
	/**
	 * @brief Update item contents
	 *
	 * @param item
	 */
	bool updateItem( Item * item );


private:
	class Private;
	Private * d;
};

#endif // MODEL_H
