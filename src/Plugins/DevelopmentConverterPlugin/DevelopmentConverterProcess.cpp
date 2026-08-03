#include "DevelopmentConverterProcess.h"

#if defined(MENGINE_PLATFORM_WINDOWS)
#   include "Environment/Windows/Win32CreateProcess.h"
#else
#   include "Environment/POSIX/POSIXCreateProcess.h"
#endif

#include "Kernel/Logger.h"

#if defined(MENGINE_PLATFORM_WINDOWS)
#   include "Kernel/UnicodeHelper.h"
#endif

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        bool executeDevelopmentConverterProcess( const FilePath & _executable, const std::vector<String> & _arguments, uint32_t * const _exitCode )
        {
            LOGGER_INFO( "convert", "execute converter command '%s'", _executable.c_str() );

#if defined(MENGINE_PLATFORM_WINDOWS)
            String command;

            for( const String & argument : _arguments )
            {
                if( command.empty() == false )
                {
                    command += ' ';
                }

                command += '"';

                for( const Char character : argument )
                {
                    if( character == '"' )
                    {
                        command += '\\';
                    }

                    command += character;
                }

                command += '"';
            }

            WString unicodeCommand;

            if( Helper::utf8ToUnicode( command, &unicodeCommand ) == false )
            {
                return false;
            }

            if( Helper::Win32CreateProcessA( _executable.c_str(), unicodeCommand.c_str(), true, _exitCode ) == false )
            {
                return false;
            }
#else
            std::vector<const Char *> argv;
            argv.reserve( _arguments.size() + 2 );
            argv.emplace_back( _executable.c_str() );

            for( const String & argument : _arguments )
            {
                argv.emplace_back( argument.c_str() );
            }

            argv.emplace_back( nullptr );

            if( Helper::POSIXCreateProcess( _executable.c_str(), argv.data(), _exitCode ) == false )
            {
                return false;
            }
#endif

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
