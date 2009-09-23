
#	pragma once

#	include <AL/al.h>

#	define OAL_CHECK_ERROR()\
 {\
 ALenum error = alGetError();\
 if( error != AL_NO_ERROR )\
	 printf( "OAL Error: (%s %d) %s\n", __FILE__, __LINE__, alGetString( error ) );\
 }
