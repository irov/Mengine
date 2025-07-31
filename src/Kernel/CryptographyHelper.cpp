#include "CryptographyHelper.h"

#include "Interface/CryptographySystemInterface.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        bool generateRandomSeed( uint64_t * const _seed )
        {
            if( CRYPTOGRAPHY_SYSTEM()
                ->generateRandomSeed( _seed ) == false )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool generateRandomHexadecimal( size_t _length, Char * const _hexadecimal, bool _lowercase )
        {
            if( CRYPTOGRAPHY_SYSTEM()
                ->generateRandomHexadecimal( _length, _hexadecimal, _lowercase ) == false )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}