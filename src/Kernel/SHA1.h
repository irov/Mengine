#pragma once

#include "Config/StdInt.h"
#include "Config/StdDef.h"
#include "Config/Char.h"

#define MENGINE_SHA1_UINT8_COUNT 20
#define MENGINE_SHA1_HEX_COUNT 40

namespace Mengine
{
    namespace Helper
    {
        void makeSHA1( const void * _buffer, size_t _size, uint8_t * const _sha1, size_t _digestSize );
        void makeSHA1HEX( const void * _buffer, size_t _size, Char * const _hex );
        void makeSHA1String( const Char * _string, Char * const _hex );
        void makeSHA1Base64( const void * _buffer, size_t _size, Char * const _base64 );
    }
}