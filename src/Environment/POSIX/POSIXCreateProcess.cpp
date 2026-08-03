#include "POSIXCreateProcess.h"

#include "Kernel/Logger.h"

#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>

namespace Mengine
{
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
                LOGGER_ERROR( "unable to create process pipe '%s': %s"
                    , _executable
                    , ::strerror( errno )
                );

                return false;
            }

            if( ::fcntl( errorPipe[0], F_SETFD, FD_CLOEXEC ) == -1
                || ::fcntl( errorPipe[1], F_SETFD, FD_CLOEXEC ) == -1 )
            {
                LOGGER_ERROR( "unable to configure process pipe '%s': %s"
                    , _executable
                    , ::strerror( errno )
                );

                ::close( errorPipe[0] );
                ::close( errorPipe[1] );

                return false;
            }

            pid_t process = ::fork();

            if( process == -1 )
            {
                LOGGER_ERROR( "unable to fork process '%s': %s"
                    , _executable
                    , ::strerror( errno )
                );

                ::close( errorPipe[0] );
                ::close( errorPipe[1] );

                return false;
            }

            if( process == 0 )
            {
                ::close( errorPipe[0] );

                ::execvp( _executable, const_cast<Char * const *>(_arguments) );

                const int executeError = errno;
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

                LOGGER_ERROR( "unable to wait process '%s': %s"
                    , _executable
                    , ::strerror( errno )
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
                LOGGER_ERROR( "unable to execute process '%s': %s"
                    , _executable
                    , ::strerror( executeError )
                );

                return false;
            }

            if( executeErrorSize == -1 )
            {
                LOGGER_ERROR( "unable to read process result '%s': %s"
                    , _executable
                    , ::strerror( errno )
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
    }
}
