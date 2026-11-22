#include "Win32CryptographySystem.h"

#include "Environment/Windows/Win32Helper.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Logger.h"
#include "Kernel/ReadHelper.h"
#include "Kernel/Hexadecimal.h"

#include "Config/StdAlgorithm.h"
#include "Config/StdString.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( CryptographySystem, Mengine::Win32CryptographySystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Win32CryptographySystem::Win32CryptographySystem()
        : m_hCryptProvider( NULL )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32CryptographySystem::~Win32CryptographySystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32CryptographySystem::_initializeService()
    {
        HCRYPTPROV hCryptProvider;
        if( ::CryptAcquireContext( &hCryptProvider
            , NULL
            , (LPCWSTR)L"Microsoft Base Cryptographic Provider v1.0"
            , PROV_RSA_FULL
            , CRYPT_VERIFYCONTEXT ) == FALSE )
        {
            return false;
        }

        m_hCryptProvider = hCryptProvider;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32CryptographySystem::_finalizeService()
    {
        if( ::CryptReleaseContext( m_hCryptProvider, 0 ) == FALSE )
        {
            LOGGER_ERROR( "CryptReleaseContext invalid %ls"
                , Helper::Win32GetLastErrorMessageW()
            );
        }

        m_hCryptProvider = NULL;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32CryptographySystem::generateRandomBytes( void * const _buffer, size_t _size ) const
    {
        BYTE * buffer = static_cast<BYTE *>(_buffer);
        size_t remaining = _size;

        while( remaining != 0 )
        {
            DWORD size = static_cast<DWORD>(StdAlgorithm::min( remaining, static_cast<size_t>(MAXDWORD) ));

            if( ::CryptGenRandom( m_hCryptProvider, size, buffer ) == FALSE )
            {
                LOGGER_ERROR( "CryptGenRandom invalid %ls"
                    , Helper::Win32GetLastErrorMessageW()
                );

                return false;
            }

            buffer += size;
            remaining -= size;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32CryptographySystem::generateRandomSeed( uint64_t * const _seed ) const
    {
        BYTE pbData[sizeof( uint64_t )];

        if( this->generateRandomBytes( pbData, sizeof(pbData) ) == false )
        {
            return false;
        }

        uint64_t seed;
        Helper::readUint64( pbData, &seed );

        *_seed = seed;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32CryptographySystem::generateRandomHexadecimal( Char * const _hexadecimal, size_t _capacity, uint32_t _flags ) const
    {
        if( _capacity == 0 )
        {
            return false;
        }

        size_t length = _capacity - 1;
        MENGINE_ASSERTION_FATAL( length < 1024, "invalid length %zu", length );
        MENGINE_ASSERTION_FATAL( length % 2 == 0, "invalid odd %zu", length );

        size_t dataSize = length / 2;
        BYTE pbData[512];

        if( this->generateRandomBytes( pbData, dataSize ) == false )
        {
            return false;
        }

        if( Helper::encodeHexadecimal( pbData, dataSize, _hexadecimal, _capacity, _flags, nullptr ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
