#pragma once

#include "Kernel/Data.h"
#include "Kernel/String.h"

#include "Config/Typedef.h"
#include "Config/Char.h"

namespace Mengine
{
    namespace Helper
    {
        size_t getBase64EncodeSize( size_t _size );
        size_t getBase64DecodeSize( const Char * _base64, size_t _size );

        void encodeBase64( const void * _data, size_t _datasize, Char * const _base64 );
        void encodeBase64( const Data & _data, String * const _base64 );

        bool decodeBase64( const Char * _base64, size_t _size, void * const _data );
        bool decodeBase64( const String & _base64, Data * const _data );
    }
}