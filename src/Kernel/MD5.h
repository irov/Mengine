#pragma once

#include "Config/StdInt.h"
#include "Config/StdDef.h"
#include "Config/Char.h"

namespace Mengine
{
    namespace Helper
    {
        void makeMD5( const void * _buffer, size_t _size, uint8_t * const _md5, size_t _md5size );
        void makeMD5HEX( const void * _buffer, size_t _size, Char * const _hex, uint32_t _flags );
        void makeMD5String( const Char * _string, Char * const _hex, uint32_t _flags );
        void makeMD5Base64( const void * _buffer, size_t _size, Char * const _base64 );
    }
}
