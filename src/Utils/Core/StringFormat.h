#pragma once

#include "Config/String.h"
#include "Config/VectorString.h"

namespace Mengine
{
    namespace Helper
    {
        uint32_t getStringFormatExpectedArgs( const String & _format );
        String getStringFormat( const String & _format, const TVectorString & _arguments );
    }
}