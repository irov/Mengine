#include "Win32CryptographySystem.h"

#include "Environment/Windows/Win32Helper.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Logger.h"
#include "Kernel/ReadHelper.h"
#include "Kernel/Hexadecimal.h"

#include "Config/StdString.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( CryptographySystem, Mengine::Win32CryptographySystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Win32CryptographySystem::Win32CryptographySystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32CryptographySystem::~Win32CryptographySystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32CryptographySystem::_initializeService()
    {
        HCRYPTPROV hCryptProv;
        if( ::CryptAcquireContext(
            &hCryptProv,
            NULL,
            (LPCWSTR)L"Microsoft Base Cryptographic Provider v1.0",
            PROV_RSA_FULL,
            CRYPT_VERIFYCONTEXT ) == FALSE )
        {
            return false;
        }

        m_hCryptProv = hCryptProv;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32CryptographySystem::_finalizeService()
    {
        if( ::CryptReleaseContext( m_hCryptProv, 0 ) == FALSE )
        {
            LOGGER_ERROR( "CryptReleaseContext invalid %ls"
                , Helper::Win32GetLastErrorMessage()
            );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32CryptographySystem::generateRandomSeed( uint64_t * const _seed ) const
    {
        BYTE pbData[sizeof( uint64_t )];
        if( ::CryptGenRandom( m_hCryptProv, sizeof( pbData ), pbData ) == FALSE )
        {
            LOGGER_ERROR( "CryptGenRandom invalid %ls"
                , Helper::Win32GetLastErrorMessage()
            );

            return false;
        }

        uint64_t seed;
        Helper::readUint64( pbData, &seed );

        *_seed = seed;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32CryptographySystem::generateRandomHexadecimal( uint32_t _length, Char * const _hexadecimal ) const
    {
        MENGINE_ASSERTION_FATAL( _length < 1024, "invalid length %u"
            , _length
        );

        MENGINE_ASSERTION_FATAL( _length % 2 == 0, "invalid odd %u"
            , _length
        );

        BYTE pbData[512];
        if( ::CryptGenRandom( m_hCryptProv, _length / 2, pbData ) == FALSE )
        {
            LOGGER_ERROR( "CryptGenRandom invalid %ls"
                , Helper::Win32GetLastErrorMessage()
            );

            return false;
        }

        Helper::encodeHexadecimal( pbData, _length / 2, _hexadecimal, _length, nullptr );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
