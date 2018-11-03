#include "Assertion.h"

#include "Kernel/Logger.h"

#include "Interface/OptionsInterface.h"
#include "Interface/ConfigServiceInterface.h"

#ifdef _WIN32
#   include "Environment/WIN32/WindowsIncluder.h"
#endif

#include <stdio.h>

namespace Mengine
{
    void Assertion( const Char * _message, const Char* _file, int _line )
    {
        Char assert_message[2048];
        snprintf( assert_message, 2048, "File [%s:%d] Assertion: '%s'"
            , _file
            , _line
            , _message );

        LOGGER_ERROR( assert_message );

        if( HAS_OPTION( "assert" ) == false && CONFIG_VALUE( "Engine", "AssertionDebugBreak", false ) == false )
        {
            return;
        }

#ifdef _WIN32
        DebugBreak();
#endif
    }
}