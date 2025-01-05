#include "newprofiledlg.h"
#include "ui_newprofiledlg.h"

#include <jsmapper/devicemap.h>

#include <QMessageBox>

/**
 * @brief Internal private class for NewProfileDlg
 */
class NewProfileDlg:: Private {
public:
	QString name;
	QString target;
	QString description;
};


//

NewProfileDlg::NewProfileDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewProfileDlg)
{
	ui->setupUi(this);
	d = new Private();

	fillTargets();	
}


NewProfileDlg::~NewProfileDlg()
{
	delete ui;
	delete d;
}

//

static bool _fillTargets( jsmapper::DeviceMap * map, void * data )
{
	QStringList * list = (QStringList *) data;
	list->append( map->getName().c_str() );
	return true;
}

void NewProfileDlg::fillTargets()
{
	QStringList targetList;
	jsmapper::DeviceMap::enumerate( _fillTargets, &targetList );
	
	qSort( targetList );
	
	foreach( QString target, targetList )
	{
		ui->targetCb->addItem( target );
	}
}


//

void NewProfileDlg::accept() // virtual
{
	d->name = ui->nameEd->text();	
	d->target = ui->targetCb->currentText();
	d->description = ui->descrEd->toPlainText();

	// validate parameters:
	bool isOK = true;	
	if( d->name.length() == 0 ) 
	{
		QMessageBox::critical( this, tr("Error"), tr("Profile name can't be empty!") );
		isOK = false;
	}
	if( isOK && (d->target.length() == 0) ) 
	{
		QMessageBox::critical( this, tr("Error"), tr("Profile target can't be empty!") );
		isOK = false;
	}
	
	// close dialog if everything's OK:
	if( isOK )
	{
		QDialog::accept();
	}
}


//

QString NewProfileDlg::getName() const
{
	return d->name;
}

QString NewProfileDlg::getTarget() const
{
	return d->target;
}

QString NewProfileDlg::getDescription() const
{
	return d->description;
}
