#include "MD5.h"

#include "Kernel/Base64.h"
#include "Kernel/Hexadecimal.h"

#include "Config/StdString.h"

#include "md5/md5.hpp"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        void makeMD5( const void * _buffer, size_t _size, uint8_t * const _md5, size_t _md5size )
        {
            md5_data( _buffer, _size, _md5, _md5size );
        }
        //////////////////////////////////////////////////////////////////////////
        void makeMD5HEX( const void * _buffer, size_t _size, Char * const _hex, uint32_t _flags )
        {
            uint8_t md5[MENGINE_MD5_UINT8_COUNT];
            Helper::makeMD5( _buffer, _size, md5, MENGINE_MD5_UINT8_COUNT );

            size_t capacity = MENGINE_MD5_HEX_COUNT + 1;
            Helper::encodeHexadecimal( md5, MENGINE_MD5_UINT8_COUNT, _hex, capacity, _flags, nullptr );
        }
        //////////////////////////////////////////////////////////////////////////
        void makeMD5String( const Char * _string, Char * const _hex, uint32_t _flags )
        {
            size_t length = StdString::strlen( _string );
            Helper::makeMD5HEX( _string, length, _hex, _flags );
        }
        //////////////////////////////////////////////////////////////////////////
        void makeMD5Base64( const void * _buffer, size_t _size, Char * const _base64 )
        {
            uint8_t md5[MENGINE_MD5_UINT8_COUNT];
            Helper::makeMD5( _buffer, _size, md5, MENGINE_MD5_UINT8_COUNT );

            Helper::encodeBase64( md5, MENGINE_MD5_UINT8_COUNT, _base64 );
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
