#include "AppleCryptographySystem.h"

#include "Kernel/Hexadecimal.h"

#include "Config/StdString.h"

#include <Security/Security.h>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( CryptographySystem, Mengine::AppleCryptographySystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleCryptographySystem::AppleCryptographySystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleCryptographySystem::~AppleCryptographySystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleCryptographySystem::_initializeService()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleCryptographySystem::_finalizeService()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleCryptographySystem::generateRandomBytes( void * const _buffer, size_t _size ) const
    {
        if( _size == 0 )
        {
            return true;
        }

        int status = ::SecRandomCopyBytes( kSecRandomDefault, _size, _buffer );

        if( status != errSecSuccess )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleCryptographySystem::generateRandomSeed( uint64_t * const _seed ) const
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
    bool AppleCryptographySystem::generateRandomHexadecimal( Char * const _hexadecimal, size_t _capacity, uint32_t _flags ) const
    {
        if( _capacity == 0 )
        {
            return false;
        }

        size_t length = _capacity - 1;
        MENGINE_ASSERTION_FATAL( length < 1024, "invalid length %zu", length );
        MENGINE_ASSERTION_FATAL( length % 2 == 0, "invalid odd %zu", length );
        
        uint8_t buffer[512];
        size_t dataSize = length / 2;

        if( this->generateRandomBytes( buffer, dataSize ) == false )
        {
            return false;
        }

        if( Helper::encodeHexadecimal( buffer, dataSize, _hexadecimal, _capacity, _flags, nullptr ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
