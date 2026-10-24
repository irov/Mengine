#include "Fingerprint.h"

#include "Interface/CryptographySystemInterface.h"

namespace Mengine
{
    namespace Helper
    {
        void fingerprintSHA1( Char * const _fingerprint, bool _lowercase )
        {
            size_t fingerprintCapacity = MENGINE_SHA1_HEX_COUNT + 1;

            CRYPTOGRAPHY_SYSTEM()
                ->generateRandomHexadecimal( _fingerprint, fingerprintCapacity, _lowercase );
        }
    }
}