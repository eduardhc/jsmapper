#ifndef __APP_H
#define __APP_H

#include <QApplication>
#include <jsmapper/common.h>

/**
 * @brief Main application class for JSMapper Studio.
 * 
 * This class holds global state for JSMapper Studio application, 
 * including the reference to the main window, the currently open 
 * profile and so on.
 */
class App: public QApplication
{
	Q_OBJECT


// construction:
public:
	/**
	 * @brief App constructor. 
	 * 
	 * @param argc
	 * @param argv
	 */
	App( int argc, char *argv[] );
	virtual ~App();
	
	/**
	 * @brief Returns App object
	 * @return 
	 */
	static App * instance();


// execution:
public:
	/**
	 * @brief Runs the application.
	 * @return Exit code
	 */
	int run();


// profile handling:
public:
	/**
	 * @brief Returns current profile
	 * @return 
	 */
	jsmapper::Profile * currentProfile() const;
	
	/**
	 * @brief Returns current profile path, if any 
	 * @return 
	 */
	QString currentProfileFile() const;
	
	
	/**
	 * @brief Creates a new profile.
	 * 
	 * Deletes current profile and creates a new one, based on provided
	 * parameters. Views are notified after the change.
	 * 
	 * @param name
	 * @param target
	 * @param description
	 */
	void createProfile( const QString &name, const QString &target, const QString &description );
	
	/**
	 * @brief Opens a profile file from disk.
	 * 
	 * Opens profile file from disk. If succesful, deletes current profile and 
	 * and replaces it with new one. Views are notified after the change.
	 * 
	 * @param file
	 */
	bool openProfile( const QString &path );

	/**
	 * @brief Saves profile to disk.
	 * 
	 * Saves currently opened profile todisk. 
	 * Views are notified after the status change.
	 * 
	 * @param file
	 */
	bool saveProfile( const QString &path );

	/**
	 * @brief Closes current profile, notifying views.
	 */
	void closeProfile();	
	
	/**
	 * @brief Returns true if profile has been modified and not yet saved
	 * @return 
	 */
	bool isModified() const;
	
	/**
	 * @brief Sets modified flag
	 * @return 
	 */
	void setModified( bool set = true );
	
	
// signals:
signals:	
	/**
	 * @brief Sent whenever a new profile becomes active
	 */
	void profileOpen( jsmapper::Profile * );
	
	/**
	 * @brief Sent whenever current profile status has changed (saved, modified, ...)
	 */
	void profileStatusChanged( jsmapper::Profile * );
	
	/**
	 * @brief Sent whenever current profile is about to be destroyed
	 */
	void profileClosed( jsmapper::Profile * );
	
	
private:
	class Private;
	Private * d;
};

#endif // APP_H
