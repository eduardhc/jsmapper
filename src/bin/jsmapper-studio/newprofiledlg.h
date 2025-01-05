#ifndef NEWPROFILEDLG_H
#define NEWPROFILEDLG_H

#include <QDialog>

namespace Ui {
	class NewProfileDlg;
}

/**
 * @brief Profile creation dialog
 */
class NewProfileDlg : public QDialog
{
	Q_OBJECT
	
public:
	explicit NewProfileDlg(QWidget *parent = 0);
	~NewProfileDlg();
	

public:
	/** 
	 * @brief Returns profile name 
	 */
	QString getName() const;
	
	/** 
	 * @brief Returns profile target 
	 */
	QString getTarget() const;
	
	/** 
	 * @brief Returns profile description (might be empty)  
	 */
	QString getDescription() const;
	
	
private:
	/** 
	 * Accept slot 
	 */
	virtual void accept();
	

private:
	/** Fills targets combobox */
	void fillTargets();

	
private:
	Ui::NewProfileDlg *ui;
	
	class Private;
	Private * d;
};

#endif // NEWPROFILEDLG_H
