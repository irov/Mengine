#pragma once

#include "Interface/ServiceInterface.h"

namespace Mengine
{
    bool s_OpenALErrorCheck( const char * _file, int _line );
}

#	define OPENAL_CHECK_ERROR() Mengine::s_OpenALErrorCheck( __FILE__, __LINE__ )

