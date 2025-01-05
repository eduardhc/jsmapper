#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <jsmapper/common.h>

namespace Ui {
	class MainWindow;
}

/**
 * @brief Main window class for JSMapper Studio
 */
class MainWindow : public QMainWindow
{
	Q_OBJECT
	
public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();
	
private slots:
	void onNew();
	void onOpen();
	bool onSave();
	bool onSaveAs();
	void onQuit();
	void onProfileOpen( jsmapper::Profile * );
	void onProfileStatusChanged( jsmapper::Profile * );
	void onProfileClosed( jsmapper::Profile * );
	
private:
	/** 
	 * Set up central slider + views 
	 */
	void initViews();

	/**
	 * @brief Tries to close current profile, asking to save changes if needed
	 * @return 
	 */
	bool tryClose();
	
	/**
	 * @brief Updates window title, according to current profile
	 */
	void updateTitle();

private:
	/**
	 * @brief Window close event. Overriden to store application settings.
	 * @param event
	 */
	virtual void closeEvent( QCloseEvent *event );

	
// settings handling:
private:
	static const char * SETTINGS_MAINWND_GROUP;
	static const char * SETTINGS_GEOMETRY;
	static const char * SETTINGS_STATE;
	static const char * SETTINGS_LAST_DIR;
	
private:
	/** Stores main window settings */
	void writeSettings() const;
	/** Reloads main window settings */
	void readSettings();
		
private:
	Ui::MainWindow *ui;
	class Private;
	Private * d;
};

#endif // MAINWINDOW_H
