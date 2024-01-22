#pragma once

#include "Config/Typedef.h"
#include "Config/Char.h"

namespace Mengine
{
    namespace Helper
    {
        size_t getBase64EncodeSize( size_t _size );
        size_t getBase64DecodeSize( const Char * _base64, size_t _size );

        void encodeBase64( const void * _data, size_t _datasize, Char * const _base64 );
        void decodeBase64( const Char * _base64, size_t _size, void * const _data );
    }
}