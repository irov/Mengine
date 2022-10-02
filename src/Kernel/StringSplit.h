#pragma once

#include "Config/Char.h"
#include "Config/StdDef.h"

#include "Config/Lambda.h"

namespace Mengine
{
    namespace Helper
    {
        typedef Lambda<void( const Char * _str, const Char * end )> LambdaStringSplit;
        void stringSplit( const Char * _in, const LambdaStringSplit & _lambda );
    }
}
