#include "Win32MessageBoxLogger.h"

#include "Environment/Windows/WindowsIncluder.h"

#include "Kernel/DebugBreak.h"

#include "Config/StdIO.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Win32MessageBoxLogger::Win32MessageBoxLogger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32MessageBoxLogger::~Win32MessageBoxLogger()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32MessageBoxLogger::log( const LoggerMessage & _message )
    {
        const Char * data = _message.data;
        size_t size = _message.size;

        Char message[MENGINE_LOGGER_MAX_MESSAGE] = {'\0'};
        MENGINE_SNPRINTF( message, MENGINE_LOGGER_MAX_MESSAGE, "%.*s"
            , size
            , data 
        );

        Helper::debuggerBreak();

        ::MessageBoxA( NULL, message, "Mengine critical error", MB_OK );
    }
    //////////////////////////////////////////////////////////////////////////
}
