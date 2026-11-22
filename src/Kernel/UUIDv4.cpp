#include "UUIDv4.h"

#include "Kernel/CryptographyHelper.h"
#include "Kernel/Hexadecimal.h"
#include "Kernel/MemoryCopy.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        bool makeUUIDv4( UUIDv4 * const _uuid, uint32_t _flags )
        {
            uint8_t bytes[MENGINE_UUID_UINT8_COUNT];

            if( Helper::generateRandomBytes( bytes, sizeof(bytes) ) == false )
            {
                return false;
            }

            bytes[6] = (bytes[6] & 0x0F) | 0x40;
            bytes[8] = (bytes[8] & 0x3F) | 0x80;

            Char hexadecimal[MENGINE_UUID_UINT8_COUNT * 2 + 1];

            if( Helper::encodeHexadecimal( bytes, sizeof(bytes), hexadecimal, sizeof(hexadecimal), _flags, nullptr ) == false )
            {
                return false;
            }

            Char * uuid = _uuid->data;

            Helper::memoryCopy( uuid, 0, hexadecimal, 0, 8 );
            uuid[8] = '-';
            Helper::memoryCopy( uuid, 9, hexadecimal, 8, 4 );
            uuid[13] = '-';
            Helper::memoryCopy( uuid, 14, hexadecimal, 12, 4 );
            uuid[18] = '-';
            Helper::memoryCopy( uuid, 19, hexadecimal, 16, 4 );
            uuid[23] = '-';
            Helper::memoryCopy( uuid, 24, hexadecimal, 20, 12 );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
