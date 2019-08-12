#pragma once

#include "Config/String.h"
#include "Config/VectorString.h"

namespace Mengine
{
    namespace Helper
    {
        uint32_t getStringFormatExpectedArgs( const Char * _format, size_t _size );
        bool getStringFormat( String & _out, const Char * _format, size_t _size, const VectorString & _arguments );
    }
}