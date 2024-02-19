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
    void Win32MessageBoxLogger::_log( const LoggerRecordInterfacePtr & _record )
    {
        Helper::debuggerBreak();

        LoggerMessage message;
        _record->getMessage( &message );

        ::MessageBoxA( NULL, message.data, "Mengine critical error", MB_OK );
    }
    //////////////////////////////////////////////////////////////////////////
}
