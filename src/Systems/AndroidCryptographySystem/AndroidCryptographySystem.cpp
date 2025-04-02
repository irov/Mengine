#include "AndroidCryptographySystem.h"

#include "Config/StdString.h"

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
    bool AndroidCryptographySystem::generateRandomHexadecimal( uint32_t _length, Char * const _hexadecimal, bool _lowercase ) const
    {
        uint32_t bytes_needed = (_length + 1) / 2;
        uint8_t random_bytes[bytes_needed];

        ::arc4random_buf( random_bytes, bytes_needed );

        if( _lowercase == false )
        {
            for( uint32_t i = 0; i != bytes_needed; ++i )
            {
                uint8_t b = random_bytes[i];

                ::sprintf( _hexadecimal + i * 2, "%02x", b );
            }
        }
        else
        {
            for( uint32_t i = 0; i != bytes_needed; ++i )
            {
                uint8_t b = random_bytes[i];

                ::sprintf( _hexadecimal + i * 2, "%02X", b );
            }
        }


        _hexadecimal[_length] = '\0';

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
