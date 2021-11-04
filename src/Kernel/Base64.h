#pragma once

#include "Config/Typedef.h"
#include "Config/Char.h"

namespace Mengine
{
    namespace Helper
    {
        size_t getBase64EncodeSize( size_t _size );
        size_t getBase64DecodeSize( const Char * _base64, size_t _size );

        bool encodeBase64( const void * _data, size_t _datasize, bool _mod, Char * const _base64, size_t _capacity, size_t * const _outsize );
        bool decodeBase64( const Char * _base64, size_t _size, void * const _data, size_t _capacity, size_t * const _outsize );
    }
}