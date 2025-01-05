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
 * \file devicelistitemdelegate.h
 * \brief Item delegate for device list items
 * \author Eduard Huguet <eduardhc@gmail.com>
 */

#ifndef ITEMDELEGATE_H
#define ITEMDELEGATE_H

#include <QStyledItemDelegate>

class MainDialog;
class Model;

/**
 * @brief Device list item delegate
 *
 * Provides editing features for items, specifiquely the dropdown
 * combobox to select a profile.
 */
class ItemDelegate : public QStyledItemDelegate
{
	Q_OBJECT
public:
	explicit ItemDelegate( MainDialog * parent, Model * model );
	virtual ~ItemDelegate();

public:
	MainDialog * mainDlg() const;


signals:
	
public slots:
	

// override:
public:
	/**
	 * @brief Creates editor for the given column
	 *
	 * @param parent
	 * @param option
	 * @param index
	 * @return
	 */
	virtual QWidget * createEditor(QWidget *parent,
								   const QStyleOptionViewItem &option,
								   const QModelIndex &index) const;

	virtual void setEditorData ( QWidget * editor,
								 const QModelIndex & index ) const;

	virtual void setModelData ( QWidget * editor,
								QAbstractItemModel * model,
								const QModelIndex & index ) const;

private:
	class Private;
	Private * d;
};

#endif // ITEMDELEGATE_H
