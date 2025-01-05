#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/joystick.h>

#define JOY_DEV "/dev/js0"

int main()
{
	struct js_event js;
	int joy_fd, num_of_axis=0, num_of_buttons=0;
	char name_of_joystick[80];
	
	joy_fd = open( JOY_DEV , O_RDONLY);
	if( joy_fd == -1 ) 
	{
		printf( "Couldn't open joystick\n" );
		return -1;
	}

	ioctl( joy_fd, JSIOCGAXES, &num_of_axis );
	ioctl( joy_fd, JSIOCGBUTTONS, &num_of_buttons );
	ioctl( joy_fd, JSIOCGNAME(80), &name_of_joystick );
	printf("Joystick detected: %s\n\t%d axis\n\t%d buttons\n\n"
		, name_of_joystick
		, num_of_axis
		, num_of_buttons );
	
	close( joy_fd );
	return 0;
}
