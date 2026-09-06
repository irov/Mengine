#include "POSIXCreateProcess.h"

#include "Kernel/Logger.h"

#include "Config/StdErrno.h"
#include "Config/StdIO.h"
#include "Config/StdLib.h"
#include "Config/StdString.h"

#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        bool POSIXCreateProcess( const Char * _executable, const Char * const * _arguments, uint32_t * const _exitCode )
        {
            LOGGER_INFO( "platform", "create process '%s'"
                , _executable
            );

            int errorPipe[2];

            if( ::pipe( errorPipe ) != 0 )
            {
                const Char * errorMessage = StdString::strerror( errno );

                LOGGER_ERROR( "unable to create process pipe '%s': %s"
                    , _executable
                    , errorMessage
                );

                return false;
            }

            for( uint32_t pipeIndex = 0; pipeIndex != 2; ++pipeIndex )
            {
                if( ::fcntl( errorPipe[pipeIndex], F_SETFD, FD_CLOEXEC ) == -1 )
                {
                    const Char * errorMessage = StdString::strerror( errno );

                    LOGGER_ERROR( "unable to configure process pipe '%s': %s"
                        , _executable
                        , errorMessage
                    );

                    ::close( errorPipe[0] );
                    ::close( errorPipe[1] );

                    return false;
                }
            }

            pid_t process = ::fork();

            if( process == -1 )
            {
                const Char * errorMessage = StdString::strerror( errno );

                LOGGER_ERROR( "unable to fork process '%s': %s"
                    , _executable
                    , errorMessage
                );

                ::close( errorPipe[0] );
                ::close( errorPipe[1] );

                return false;
            }

            if( process == 0 )
            {
                ::close( errorPipe[0] );

                Char * const * arguments = const_cast<Char * const *>(_arguments);

                ::execvp( _executable, arguments );

                int executeError = errno;
                (void)::write( errorPipe[1], &executeError, sizeof( executeError ) );

                ::_exit( 127 );
            }

            ::close( errorPipe[1] );

            int status = 0;

            while( ::waitpid( process, &status, 0 ) == -1 )
            {
                if( errno == EINTR )
                {
                    continue;
                }

                const Char * errorMessage = StdString::strerror( errno );

                LOGGER_ERROR( "unable to wait process '%s': %s"
                    , _executable
                    , errorMessage
                );

                ::close( errorPipe[0] );

                return false;
            }

            int executeError = 0;
            ssize_t executeErrorSize;

            do
            {
                executeErrorSize = ::read( errorPipe[0], &executeError, sizeof( executeError ) );
            } while( executeErrorSize == -1 && errno == EINTR );

            ::close( errorPipe[0] );

            if( executeErrorSize == sizeof( executeError ) )
            {
                const Char * errorMessage = StdString::strerror( executeError );

                LOGGER_ERROR( "unable to execute process '%s': %s"
                    , _executable
                    , errorMessage
                );

                return false;
            }

            if( executeErrorSize == -1 )
            {
                const Char * errorMessage = StdString::strerror( errno );

                LOGGER_ERROR( "unable to read process result '%s': %s"
                    , _executable
                    , errorMessage
                );

                return false;
            }

            if( executeErrorSize != 0 )
            {
                LOGGER_ERROR( "invalid process result '%s' size: %zd"
                    , _executable
                    , executeErrorSize
                );

                return false;
            }

            uint32_t exitCode;

            if( WIFEXITED( status ) != 0 )
            {
                exitCode = static_cast<uint32_t>( WEXITSTATUS( status ) );
            }
            else if( WIFSIGNALED( status ) != 0 )
            {
                exitCode = static_cast<uint32_t>( 128 + WTERMSIG( status ) );
            }
            else
            {
                exitCode = EXIT_FAILURE;
            }

            LOGGER_INFO( "platform", "process result '%s' [%u]"
                , _executable
                , exitCode
            );

            if( _exitCode != nullptr )
            {
                *_exitCode = exitCode;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool POSIXReadProcessOutput( const Char * _command, Char * const _output, size_t _capacity )
        {
            if( _capacity == 0 )
            {
                return false;
            }

            _output[0] = '\0';

            StdIO::FILE * pipe = ::popen( _command, "r" );

            if( pipe == nullptr )
            {
                return false;
            }

            size_t outputCapacity = _capacity - 1;
            size_t outputSize = StdIO::fread( _output, 1, outputCapacity, pipe );
            _output[outputSize] = '\0';

            bool overflow = false;
            constexpr size_t discardCapacity = 1024;
            Char discard[discardCapacity];

            for( ;; )
            {
                size_t discardSize = StdIO::fread( discard, 1, discardCapacity, pipe );

                if( discardSize == 0 )
                {
                    break;
                }

                overflow = true;
            }

            int readError = StdIO::ferror( pipe );
            int closeResult = ::pclose( pipe );

            if( readError != 0 )
            {
                return false;
            }

            if( overflow == true )
            {
                return false;
            }

            if( closeResult == -1 )
            {
                return false;
            }

            if( WIFEXITED( closeResult ) == 0 )
            {
                return false;
            }

            if( WEXITSTATUS( closeResult ) != EXIT_SUCCESS )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
}
