#include "Assertion.h"

#include "Kernel/Logger.h"

#ifdef _WIN32
#   include "WIN32/WindowsIncluder.h"
#endif

#include <stdio.h>

namespace Mengine
{
#ifndef NDEBUG
    void Assertion( const Char * _message, const Char* _file, int _line )
    {
        Char assert_message[2048];
        snprintf( assert_message, 2048, "File [%s:%d] Assertion: '%s'"
            , _file
            , _line
            , _message );

        LOGGER_ERROR( assert_message );


#ifdef _WIN32
        DebugBreak();
#endif
    }
#endif
}