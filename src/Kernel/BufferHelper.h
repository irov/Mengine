#pragma once

#include "Config/Typedef.h"
#include "Config/Char.h"

namespace Mengine
{
    namespace Helper
    {
        size_t bufferFormat( Char * _buffer, size_t _capacity, MENGINE_CHECK_FORMAT_STRING( const Char * _format ), ... );
    }
}