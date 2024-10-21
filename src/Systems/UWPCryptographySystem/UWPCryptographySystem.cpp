#include "UWPCryptographySystem.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Logger.h"
#include "Kernel/ReadHelper.h"
#include "Kernel/Hexadecimal.h"

#include "Config/StdString.h"

#include <winrt/Windows.Security.Cryptography.h>
#include <winrt/Windows.Security.Cryptography.Core.h>
#include <winrt/Windows.Storage.Streams.h>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( CryptographySystem, Mengine::UWPCryptographySystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    UWPCryptographySystem::UWPCryptographySystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    UWPCryptographySystem::~UWPCryptographySystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool UWPCryptographySystem::_initializeService()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void UWPCryptographySystem::_finalizeService()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool UWPCryptographySystem::generateRandomSeed( uint64_t * const _seed ) const
    {
        auto buffer = winrt::Windows::Security::Cryptography::CryptographicBuffer::GenerateRandom( sizeof( uint64_t ) );

        uint32_t length = buffer.Length();

        if( length != sizeof( uint64_t ) )
        {
            LOGGER_ERROR( "Failed to generate the correct size of random data" );

            return false;
        }

        uint64_t seed = 0;
        auto reader = winrt::Windows::Storage::Streams::DataReader::FromBuffer( buffer );
        reader.ReadBytes( winrt::array_view<uint8_t>( reinterpret_cast<uint8_t *>(&seed), sizeof( uint64_t ) ) );

        *_seed = seed;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool UWPCryptographySystem::generateRandomHexadecimal( uint32_t _length, Char * const _hexadecimal, bool _lowercase ) const
    {
        MENGINE_ASSERTION_FATAL( _length < 1024, "invalid length %u", _length );
        MENGINE_ASSERTION_FATAL( _length % 2 == 0, "invalid odd %u", _length );

        auto buffer = winrt::Windows::Security::Cryptography::CryptographicBuffer::GenerateRandom( _length / 2 );

        uint32_t length = buffer.Length();

        if( length != _length / 2 )
        {
            LOGGER_ERROR( "Failed to generate the correct size of random data" );

            return false;
        }

        BYTE pbData[512];
        auto reader = winrt::Windows::Storage::Streams::DataReader::FromBuffer( buffer );
        reader.ReadBytes( winrt::array_view<uint8_t>( pbData, _length / 2 ) );

        Helper::encodeHexadecimal( pbData, _length / 2, _hexadecimal, _length, _lowercase, nullptr );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
