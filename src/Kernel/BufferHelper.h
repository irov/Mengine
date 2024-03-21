#pragma once

#include "Config/Typedef.h"
#include "Config/Char.h"

namespace Mengine
{
    namespace Helper
    {
        size_t bufferFormat( Char * const _buffer, size_t _capacity, const Char * _format, ... ) MENGINE_ATTRIBUTE_FORMAT_STRING( 3, 4 );
    }
}