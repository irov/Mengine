#pragma once

#include "Kernel/Data.h"
#include "Kernel/String.h"

#include "Config/Typedef.h"
#include "Config/Char.h"

namespace Mengine
{
    namespace Helper
    {
        bool encodeHexadecimal( const void * _data, size_t _datasize, Char * const _hexadecimal, size_t _capacity, bool _lowercase, size_t * const _outsize );
        bool encodeHexadecimal( const Data & _data, String * const _hexadecimal, bool _lowercase );

        bool decodeHexadecimal( const Char * _hexadecimal, size_t _size, void * const _data, size_t _capacity, size_t * const _outsize );
        bool decodeHexadecimal( const String & _hexadecimal, Data * const _data );
    }
}