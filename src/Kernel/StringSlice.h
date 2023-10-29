#pragma once

#include "Config/Char.h"
#include "Config/StdDef.h"

#include "Config/Lambda.h"

namespace Mengine
{
    namespace Helper
    {
        typedef Lambda<void( const Char * _str )> LambdaStringSlice;
        void stringSlice( const Char * _in, size_t _size, Char * _buffer, size_t _capacity, const LambdaStringSlice & _lambda );
    }
}
