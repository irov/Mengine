#include "DevelopmentConverterProcess.h"

#if defined(MENGINE_PLATFORM_WINDOWS)
#   include "Environment/Windows/Win32CreateProcess.h"
#else
#   include "Environment/POSIX/POSIXCreateProcess.h"
#endif

#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        bool executeDevelopmentConverterProcess( const FilePath & _executable, const ArgumentStrings & _arguments, uint32_t * const _exitCode )
        {
            const Char * executable = _executable.c_str();

            LOGGER_INFO( "convert", "execute converter command '%s'", executable );

#if defined(MENGINE_PLATFORM_WINDOWS)
            bool successful = Helper::Win32CreateProcess( executable, _arguments, _exitCode );

            return successful;
#else
            const Char * argv[MENGINE_MAX_PROCESS_ARGUMENTS + 2];
            argv[0] = executable;

            for( size_t index = 0; index != _arguments.size(); ++index )
            {
                argv[index + 1] = _arguments[index];
            }

            argv[_arguments.size() + 1] = nullptr;

            bool successful = Helper::POSIXCreateProcess( executable, argv, _exitCode );

            return successful;
#endif
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
