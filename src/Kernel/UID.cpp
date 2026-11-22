#include "UID.h"

#include "Kernel/CryptographyHelper.h"

#include "Config/StdDef.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        static const Char UID_ALPHABET[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
        //////////////////////////////////////////////////////////////////////////
        bool makeUID( uint32_t _length, Char * const _uid )
        {
            // Six bits cover the alphabet with two extra indices to discard.
            uint8_t symbolMask = 0x3F;
            uint8_t randomBytes[64];
            uint32_t offset = 0;

            while( offset != _length )
            {
                if( Helper::generateRandomBytes( randomBytes, sizeof(randomBytes) ) == false )
                {
                    return false;
                }

                for( size_t index = 0; index != sizeof(randomBytes) && offset != _length; ++index )
                {
                    uint8_t symbol = randomBytes[index] & symbolMask;

                    if( symbol >= MENGINE_STATIC_STRING_LENGTH( UID_ALPHABET ) )
                    {
                        continue;
                    }

                    _uid[offset++] = UID_ALPHABET[symbol];
                }
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}