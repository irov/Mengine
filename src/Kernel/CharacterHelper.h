#pragma once

#include "Config/Char.h"

namespace Mengine
{
    namespace Helper
    {
        bool isASCIICharacterSet( Char _ch );
        bool isStandardCharacterSet( Char _ch );
        bool isLowerCharacterSet( Char _ch );
        bool isUpperCharacterSet( Char _ch );
        bool isDigitCharacterSet( Char _ch );
    }
}