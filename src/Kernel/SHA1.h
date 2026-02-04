#pragma once

#include "Config/StdInt.h"
#include "Config/StdDef.h"
#include "Config/Char.h"

namespace Mengine
{
    namespace Helper
    {
        void makeSHA1( const void * _buffer, size_t _size, uint8_t * const _sha1, size_t _sha1size );
        void makeSHA1HEX( const void * _buffer, size_t _size, Char * const _hex, bool _lowercase );
        void makeSHA1String( const Char * _string, Char * const _hex, bool _lowercase );
        void makeSHA1Base64( const void * _buffer, size_t _size, Char * const _base64 );
    }
}