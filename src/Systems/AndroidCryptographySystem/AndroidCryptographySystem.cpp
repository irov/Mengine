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
    bool AndroidCryptographySystem::generateRandomBytes( void * const _buffer, size_t _size ) const
    {
        if( _size == 0 )
        {
            return true;
        }

        ::arc4random_buf( _buffer, _size );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidCryptographySystem::generateRandomSeed( uint64_t * const _seed ) const
    {
        uint64_t seed;

        if( this->generateRandomBytes( &seed, sizeof(seed) ) == false )
        {
            return false;
        }

        *_seed = seed;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidCryptographySystem::generateRandomHexadecimal( Char * const _hexadecimal, size_t _capacity, uint32_t _flags ) const
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

        if( this->generateRandomBytes( randomBytes, randomSize ) == false )
        {
            return false;
        }

        if( Helper::encodeHexadecimal( randomBytes, randomSize, _hexadecimal, _capacity, _flags, nullptr ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
