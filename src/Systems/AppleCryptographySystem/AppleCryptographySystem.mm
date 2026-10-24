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
    bool AppleCryptographySystem::generateRandomSeed( uint64_t * const _seed ) const
    {
        union {
            SInt8 bytes[sizeof(uint64_t)];
            uint64_t randomNumber = 0;
        };
        
        int status = ::SecRandomCopyBytes( kSecRandomDefault, sizeof(bytes), bytes );
        
        if( status != errSecSuccess )
        {
            return false;
        }
        
        *_seed = randomNumber;
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleCryptographySystem::generateRandomHexadecimal( Char * const _hexadecimal, size_t _capacity, bool _lowercase ) const
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
        int result = ::SecRandomCopyBytes( kSecRandomDefault, dataSize, buffer );
            
        if( result != errSecSuccess)
        {
            return false;
        }
        
        if( Helper::encodeHexadecimal( buffer, dataSize, _hexadecimal, _capacity, _lowercase, nullptr ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
