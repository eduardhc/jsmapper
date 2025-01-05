#ifndef __JSMAPPER_STUDIO_ACTIONSMODEL_H_
#define __JSMAPPER_STUDIO_ACTIONSMODEL_H_

#include <jsmapper/common.h>
#include <QAbstractTableModel>

/**
 * @brief Profile actions list data model
 * 
 * This class handles data model for profile action list.
 */
class ActionsModel : public QAbstractTableModel
{	
	Q_OBJECT
	
public:
	/**
	 * @brief Class constructor
	 */
	ActionsModel();
	virtual ~ActionsModel();
	

public:
	/** Number of columns */
	static const int NUM_COLS	= 3;
	/** Action name column */
	static const int COL_NAME	= 0;
	/** Action's action column */
	static const int COL_ACTION = 1;
	/** Action's filter flag */
	static const int COL_FILTER = 2;


public:	
	/**
	 * @brief Returns column count 
	 * @param parent
	 * @return 
	 */
	virtual int columnCount(const QModelIndex &parent) const;
	
	/**
	 * @brief Returns row count
	 * @param parent
	 * @return 
	 */
	virtual int rowCount(const QModelIndex &parent) const;	

	/**
	 * @brief Returns header data
	 * @param section
	 * @param orientation
	 * @param role
	 * @return 
	 */
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;
	
	/**
	 * @brief Returns row data
	 * @param index
	 * @param role
	 * @return 
	 */
	virtual QVariant data(const QModelIndex &index, int role) const;
	

protected:
	/**
	 * @brief Returns action text, according to its type.
	 * 
	 * @param action
	 * @return 
	 */
	static QString getActionText( const jsmapper::Action * action );
	
	/**
	 * @brief Helper function used to build action text for most common actions 
	 * @param itemName
	 * @param modifiersName
	 * @param single
	 * @return 
	 */
	static QString buildActionText( const std::string &itemName, 
									const std::list<std::string> &modifierNames, 
									bool single = false );
	
	
public slots:
	/**
	 * @brief Profile open slot notification
	 */
	void onProfileOpen( jsmapper::Profile * );
	
	/**
	 * @brief Profile close slot notification
	 */
	void onProfileClosed( jsmapper::Profile * );
	

private:
	class Private;
	Private * d;
};

#endif // __JSMAPPER_STUDIO_ACTIONSMODEL_H_

