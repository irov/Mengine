#include "POSIXCryptographySystem.h"

#include "Kernel/Assertion.h"
#include "Kernel/Hexadecimal.h"

#include "Config/Config.h"

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#if defined(MENGINE_PLATFORM_LINUX)
#   include <sys/syscall.h>
#endif

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( CryptographySystem, Mengine::POSIXCryptographySystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static bool readRandomDevice( uint8_t * const _buffer, size_t _size )
        {
            int descriptor;

            do
            {
                int openFlags = O_RDONLY;

#if defined(O_CLOEXEC)
                openFlags |= O_CLOEXEC;
#endif

                descriptor = ::open( "/dev/urandom", openFlags );
            } while( descriptor == -1 && errno == EINTR );

            if( descriptor == -1 )
            {
                return false;
            }

            size_t offset = 0;

            while( offset != _size )
            {
                ssize_t readSize = ::read( descriptor, _buffer + offset, _size - offset );

                if( readSize > 0 )
                {
                    offset += (size_t)readSize;

                    continue;
                }

                if( readSize == -1 && errno == EINTR )
                {
                    continue;
                }

                ::close( descriptor );

                return false;
            }

            ::close( descriptor );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        static bool generateRandomBytes( void * const _buffer, size_t _size )
        {
            if( _size == 0 )
            {
                return true;
            }

            uint8_t * bytes = static_cast<uint8_t *>(_buffer);
            size_t offset = 0;

#if defined(SYS_getrandom)
            while( offset != _size )
            {
                ssize_t randomSize = ::syscall( SYS_getrandom, bytes + offset, _size - offset, 0 );

                if( randomSize > 0 )
                {
                    offset += (size_t)randomSize;

                    continue;
                }

                if( randomSize == -1 && errno == EINTR )
                {
                    continue;
                }

                break;
            }

            if( offset == _size )
            {
                return true;
            }
#endif

            return Detail::readRandomDevice( bytes + offset, _size - offset );
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    POSIXCryptographySystem::POSIXCryptographySystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    POSIXCryptographySystem::~POSIXCryptographySystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool POSIXCryptographySystem::_initializeService()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void POSIXCryptographySystem::_finalizeService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool POSIXCryptographySystem::generateRandomSeed( uint64_t * const _seed ) const
    {
        uint64_t seed;

        if( Detail::generateRandomBytes( &seed, sizeof(seed) ) == false )
        {
            return false;
        }

        *_seed = seed;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool POSIXCryptographySystem::generateRandomHexadecimal( size_t _length, Char * const _hexadecimal, bool _lowercase ) const
    {
        MENGINE_ASSERTION_FATAL( _length < 1024, "invalid length %zu", _length );
        MENGINE_ASSERTION_FATAL( _length % 2 == 0, "invalid odd %zu", _length );

        uint8_t randomBytes[512];
        size_t randomSize = _length / 2;

        if( Detail::generateRandomBytes( randomBytes, randomSize ) == false )
        {
            return false;
        }

        return Helper::encodeHexadecimal( randomBytes, randomSize, _hexadecimal, _length, _lowercase, nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
}
