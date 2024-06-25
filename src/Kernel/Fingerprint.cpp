#include "Fingerprint.h"

#include "Interface/CryptographySystemInterface.h"

namespace Mengine
{
    namespace Helper
    {
        void fingerprintSHA1( Char * const _fingerprint, bool _lowercase )
        {
            CRYPTOGRAPHY_SYSTEM()
                ->generateRandomHexadecimal( MENGINE_SHA1_HEX_COUNT, _fingerprint, _lowercase );
        }
    }
}