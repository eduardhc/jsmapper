#ifndef __JSMAPPER_STUDIO_ACTIONSVIEW_H_
#define __JSMAPPER_STUDIO_ACTIONSVIEW_H_

#include <QWidget>

namespace Ui {
	class ActionsView;
}

/**
 * @brief Actions View
 * 
 * This view hols and manipulate the list of actions defined in
 * the profile.
 */
class ActionsView : public QWidget
{
	Q_OBJECT
	
public:
	explicit ActionsView(QWidget *parent = 0);
	~ActionsView();
	
private:
	Ui::ActionsView *ui;
	
	class Private;
	Private * d;
};

#endif // __JSMAPPER_STUDIO_ACTIONSVIEW_H_
