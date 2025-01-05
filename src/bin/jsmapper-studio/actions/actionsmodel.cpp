#include "actionsmodel.h"

#include <jsmapper/profile.h>
#include <jsmapper/axisaction.h>
#include <jsmapper/buttonaction.h>
#include <jsmapper/keyaction.h>
#include <jsmapper/macroaction.h>
#include <jsmapper/nullaction.h>
#include <jsmapper/keymap.h>

#include <QList>
#include <QString>


/**
 * @brief Internal ActionsModel::Private class
 */
class ActionsModel::Private 
{
public:
	/** Pointer to currently opened profile */
	jsmapper::Profile * profile;
	/** Action list names */
	QList<QString> actions;
	
public:
	Private()
		: profile( NULL )
	{
	}
};



//

ActionsModel::ActionsModel()
{
	d = new Private();
}

ActionsModel::~ActionsModel()
{
	delete d;
	d =	NULL;
}


//
// Model overrides:
//

int /*virtual*/ ActionsModel::columnCount(const QModelIndex &parent) const
{
	return NUM_COLS;	
}

//

int /*virtual*/ ActionsModel::rowCount(const QModelIndex &parent) const
{
	return d->actions.size();
}


//

QVariant /*virtual*/ ActionsModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	QVariant result;

	if( (orientation == Qt::Horizontal) )
	{
		switch( role )
		{
		case Qt::DisplayRole:
			switch( section )
			{
			case COL_NAME:
				result = tr( "Name" );
				break;
				
			case COL_ACTION:
				result = tr( "Action" );
				break;
				
			case COL_FILTER:
				result = tr( "Filter" );
				break;
				
			default:
				break;
			}
			break;
		
		default:
			break;
		}
	}
	
	return result;	
}


//

QVariant /*virtual*/ ActionsModel::data(const QModelIndex &index, int role) const
{
	QVariant result;
	
	int row = index.row();
	if( row >= 0 && row < d->actions.size() ) 
	{
		int column = index.column();
		
		const QString &name = d->actions.at( row );
		jsmapper::Action * action = d->profile->getAction( name.toStdString() );
		
		switch( role )
		{
		case Qt::DisplayRole:
			switch( column )
			{
			case COL_NAME:
				result = name;	
				break;
				
			case COL_ACTION:
				if( action != NULL )
				{
					result = getActionText( action );
				}
				break;
				
			case COL_FILTER:
				// nothing, as handled by CheckStateRole
				break;	
				
			default:
				break;
			}
			break;
		
			
		case Qt::ToolTipRole:
			if( action != NULL )
			{
				result = QString::fromStdString( action->getDescription() );
			}
			break;
			
			
		case Qt::CheckStateRole:
			if( (action != NULL) && (column == COL_FILTER) )
			{
				result = action->filter()? Qt::Checked : Qt::Unchecked;
			}
			break;
			
	
		case Qt::TextAlignmentRole:
			if( column == COL_NAME )
			{
				result = (int) Qt::AlignLeft | Qt::AlignVCenter;
			}
			else
			{
				result = (int) Qt::AlignHCenter | Qt::AlignVCenter;
			}
			break;

			
		default:
			break;
		}
	}
	
	return result;
}


//

QString /*static*/ ActionsModel::getActionText( const jsmapper::Action * action )
{
	QString result;

	jsmapper::KeyMap * keyMap = jsmapper::KeyMap::instance();
	
	switch( action->getType() )
	{
	case jsmapper::AxisActionType:
	{
		const jsmapper::AxisAction * axisAction = static_cast<const jsmapper::AxisAction *>( action );
		result = tr("**** TODO ****");
		break;
	}
	
	case jsmapper::ButtonActionType:
	{
		const jsmapper::ButtonAction * buttonAction = static_cast<const jsmapper::ButtonAction *>( action );
		std::string keyName = keyMap->getButtonSymbol( buttonAction->getButton() );
		std::list<std::string> modifierNames = keyMap->getModifiersSymbols( buttonAction->getModifiers() );
		result = buildActionText( keyName, modifierNames, buttonAction->isSingle() );
		break;
	}
	
	case jsmapper::KeyActionType:
	{
		const jsmapper::KeyAction * keyAction = static_cast<const jsmapper::KeyAction *>( action );
		std::string keyName = keyMap->getKeySymbol( keyAction->getKey() );
		std::list<std::string> modifierNames = keyMap->getModifiersSymbols( keyAction->getModifiers() );
		result = buildActionText( keyName, modifierNames, keyAction->isSingle() );
		break;
	}
	
	case jsmapper::MacroActionType:
	{
		const jsmapper::MacroAction * MacroAction = static_cast<const jsmapper::MacroAction *>( action );
		result = tr("**** TODO ****");
		break;
	}
	
	case jsmapper::NullActionType:
		result = tr( "(none)" );
		break;
	}
	
	return result;
}


//


QString /*static*/ ActionsModel::buildActionText( const std::string &itemName, 
													const std::list<std::string> &modifierNames, 
													bool single /*false*/ )
{
	QString result;

	// modifiers go first:
	foreach( std::string modifierName, modifierNames )
	{
		if( result.isEmpty() == false )
		{
			result.append( "+" );
		}
		result.append( QString::fromStdString( modifierName ) );
	}

	
	// followed by item name:
	if( result.isEmpty() == false )
	{
		result.append( "+" );
	}
	result.append( QString::fromStdString( itemName ) );
	
	
	// single flag, finally:
	if( single ) 
	{
		result.append( tr(" (1)") );
	}
	
	return result;	
}


//
// Slot notifications:
//

void ActionsModel::onProfileOpen( jsmapper::Profile * profile )
{
	d->profile = profile;
	if( d->profile != NULL )
	{
		beginResetModel();
		
		std::list<std::string> names = d->profile->getActionNames();
		std::list<std::string>::const_iterator it = names.begin();
		while( it != names.end() )
		{
			d->actions.append( QString::fromStdString( *it++ ) );
		}
		
		endResetModel();
	}
}

void ActionsModel::onProfileClosed( jsmapper::Profile * profile )
{
	if( d->profile == profile ) // it should always be, but still
	{
		beginResetModel();
		
		d->actions.clear();
		d->profile = NULL;
		
		endResetModel();
	}
}
