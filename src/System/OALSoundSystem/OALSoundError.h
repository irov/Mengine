#pragma once

#include "Interface/ServiceInterface.h"

namespace Mengine
{
	bool s_OALErrorCheck( const char * _file, int _line );
}

#	define OAL_CHECK_ERROR() Mengine::s_OALErrorCheck( __FILE__, __LINE__ )

