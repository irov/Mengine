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
    bool AppleCryptographySystem::generateRandomHexadecimal( size_t _length, Char * const _hexadecimal, bool _lowercase ) const
    {
        MENGINE_ASSERTION_FATAL( _length < 1024, "invalid length %zu", _length );
        MENGINE_ASSERTION_FATAL( _length % 2 == 0, "invalid odd %zu", _length );
        
        uint8_t buffer[512];
        int result = ::SecRandomCopyBytes( kSecRandomDefault, sizeof(buffer), buffer );
            
        if( result != errSecSuccess)
        {
            return false;
        }
        
        Helper::encodeHexadecimal( buffer, _length / 2, _hexadecimal, _length, _lowercase, nullptr );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
