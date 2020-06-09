#pragma once

#include "Config/Typedef.h"
#include "Config/Char.h"

namespace Mengine
{
    namespace Helper
    {
        size_t base64_encode_size( size_t _size );
        size_t base64_decode_size( const Char * _base64, size_t _size );

        bool base64_encode( const uint8_t * _data, size_t _datasize, bool _mod, Char * const _base64, size_t _capacity, size_t * const _outsize );
        bool base64_decode( const Char * _base64, size_t _size, uint8_t * const _data, size_t _capacity, size_t * const _outsize );
    }
}