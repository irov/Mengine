#pragma once

#include "Config/Typedef.h"
#include "Config/Char.h"

namespace Mengine
{
    namespace Helper
    {
        bool encodeHexadecimal( const void * _data, size_t _datasize, Char * const _hexadecimal, size_t _capacity, size_t * const _outsize );
        bool decodeHexadecimal( const Char * _hexadecimal, size_t _size, void * const _data, size_t _capacity, size_t * const _outsize );
    }
}