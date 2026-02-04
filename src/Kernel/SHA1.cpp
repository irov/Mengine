#include "SHA1.h"

#include "Kernel/Base64.h"
#include "Kernel/Hexadecimal.h"

#include "Config/StdString.h"
#include "Config/StdIO.h"

#include "sha1/sha1.hpp"
#include "stdex/memorycopy.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        void makeSHA1( const void * _buffer, size_t _size, uint8_t * const _sha1, size_t _sha1size )
        {
            sha1_data( _buffer, _size, _sha1, _sha1size );
        }
        //////////////////////////////////////////////////////////////////////////
        void makeSHA1HEX( const void * _buffer, size_t _size, Char * const _hex, bool _lowercase )
        {
            uint8_t sha1[MENGINE_SHA1_UINT8_COUNT];
            Helper::makeSHA1( _buffer, _size, sha1, MENGINE_SHA1_UINT8_COUNT );

            Helper::encodeHexadecimal( sha1, sizeof( sha1 ), _hex, MENGINE_UNKNOWN_SIZE, _lowercase, nullptr );
        }
        //////////////////////////////////////////////////////////////////////////
        void makeSHA1String( const Char * _string, Char * const _hex, bool _lowercase )
        {
            size_t len = StdString::strlen( _string );

            uint8_t sha1[MENGINE_SHA1_UINT8_COUNT];
            Helper::makeSHA1( _string, len, sha1, MENGINE_SHA1_UINT8_COUNT );

            Helper::encodeHexadecimal( sha1, sizeof( sha1 ), _hex, MENGINE_UNKNOWN_SIZE, _lowercase, nullptr );
        }
        //////////////////////////////////////////////////////////////////////////
        void makeSHA1Base64( const void * _buffer, size_t _size, Char * const _base64 )
        {
            uint8_t sha1[MENGINE_SHA1_UINT8_COUNT];
            Helper::makeSHA1( _buffer, _size, sha1, MENGINE_SHA1_UINT8_COUNT );

            Helper::encodeBase64( sha1, sizeof( sha1 ), _base64 );
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
