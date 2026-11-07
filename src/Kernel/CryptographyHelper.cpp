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
        bool generateRandomHexadecimal( Char * const _hexadecimal, size_t _capacity, uint32_t _flags )
        {
            if( CRYPTOGRAPHY_SYSTEM()
                ->generateRandomHexadecimal( _hexadecimal, _capacity, _flags ) == false )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}