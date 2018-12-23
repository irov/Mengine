#pragma once

#include "Interface/ServiceInterface.h"

namespace Mengine
{
    namespace Helper
    {
        bool OpenALErrorCheck( const Char * _file, uint32_t _line );
    }
}
//////////////////////////////////////////////////////////////////////////
#define OPENAL_CHECK_ERROR() Mengine::Helper::OpenALErrorCheck( __FILE__, __LINE__ )
//////////////////////////////////////////////////////////////////////////

