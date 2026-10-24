#include "AndroidCryptographySystem.h"

#include "Kernel/Assertion.h"
#include "Kernel/Hexadecimal.h"

#include <stdlib.h>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( CryptographySystem, Mengine::AndroidCryptographySystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AndroidCryptographySystem::AndroidCryptographySystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidCryptographySystem::~AndroidCryptographySystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidCryptographySystem::_initializeService()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidCryptographySystem::_finalizeService()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidCryptographySystem::generateRandomSeed( uint64_t * const _seed ) const
    {
        uint64_t rnd;
        ::arc4random_buf( &rnd, sizeof(rnd) );

        *_seed = rnd;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidCryptographySystem::generateRandomHexadecimal( Char * const _hexadecimal, size_t _capacity, bool _lowercase ) const
    {
        if( _capacity == 0 )
        {
            return false;
        }

        size_t length = _capacity - 1;
        MENGINE_ASSERTION_FATAL( length < 1024, "invalid length %zu", length );
        MENGINE_ASSERTION_FATAL( length % 2 == 0, "invalid odd %zu", length );

        uint8_t randomBytes[512];
        size_t randomSize = length / 2;
        ::arc4random_buf( randomBytes, randomSize );

        if( Helper::encodeHexadecimal( randomBytes, randomSize, _hexadecimal, _capacity, _lowercase, nullptr ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
