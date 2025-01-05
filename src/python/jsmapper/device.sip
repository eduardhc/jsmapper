%Module jsmapper

namespace jsmapper
{
	class Device
	{
	%TypeHeaderCode
	#include <jsmapper/device.h>
	%End

	public:
		Device( int id = 0 );
		
	public:
		long getVersion();
		
		int getNumAxes();
		
		int getNumButtons();
	};
};

