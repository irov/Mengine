#pragma once

#include "Config/String.h"
#include "Config/VectorString.h"

namespace Mengine
{
    namespace Helper
    {
        uint32_t getStringFormatExpectedArgs( const String & _format );
        bool getStringFormat( String & _out, const String & _format, const VectorString & _arguments );
    }
}