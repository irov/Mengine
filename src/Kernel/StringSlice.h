#pragma once

#include "Config/Char.h"
#include "Config/Utf8.h"
#include "Config/StdDef.h"

#include "Config/Lambda.h"

namespace Mengine
{
    namespace Helper
    {
        typedef Lambda<bool( const Char * _str )> LambdaStringSlice;
        typedef Lambda<bool( const Utf8 * _str )> LambdaStringUtf8Slice;

        bool stringSlice( const Char * _in, size_t _size, Char * _buffer, size_t _capacity, const LambdaStringSlice & _lambda );
        bool utf8Slice( const Utf8 * _in, size_t _size, Utf8 * _buffer, size_t _capacity, const LambdaStringUtf8Slice & _lambda );
    }
}
