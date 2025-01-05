#include "actionsview.h"
#include "actionsmodel.h"
#include "ui_actionsview.h"

#include "../app.h"


/**
 * @brief Internal ActionsView::Private class
 */
class ActionsView::Private {
public:
	ActionsModel * model;
	
public:
	Private() : 
		model( NULL ) 
	{
	}
};


//

ActionsView::ActionsView(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::ActionsView)
{
	ui->setupUi( this );
	
	d = new Private();
	d->model = new ActionsModel();

	App * app =	App::instance();
	connect( app, SIGNAL(profileOpen(jsmapper::Profile*)), d->model, SLOT(onProfileOpen(jsmapper::Profile*)) );
	connect( app, SIGNAL(profileClosed(jsmapper::Profile*)), d->model, SLOT(onProfileClosed(jsmapper::Profile*)) );
	
	ui->actionsTable->setModel( d->model );
}

//

ActionsView::~ActionsView()
{
	ui->actionsTable->setModel( NULL );
	delete d->model;
	delete d;
	d = NULL;
	
	delete ui;
	ui = NULL;
}
