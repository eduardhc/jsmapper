#include "mainwindow.h"
#include "app.h"

int main(int argc, char *argv[])
{
	App::setApplicationName( "JSMapperStudio" );
	App::setOrganizationName("JSMapper" );
	
	App app( argc, argv );
	return app.run();
}
