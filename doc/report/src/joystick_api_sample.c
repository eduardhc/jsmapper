#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/joystick.h>

#define JOY_DEV "/dev/js0"

int main()
{
	struct js_event js;
	int joy_fd;
	
	joy_fd = open( JOY_DEV , O_RDONLY);
	if( joy_fd == -1 ) 
	{
		printf( "Couldn't open joystick\n" );
		return -1;
	}

	while( 1 ) 	/* infinite loop */
	{
		/* read next joystick event */
		read(joy_fd, &js, sizeof(struct js_event));
		/* do domething useful with it... */
	}

	close( joy_fd );
	return 0;
}
