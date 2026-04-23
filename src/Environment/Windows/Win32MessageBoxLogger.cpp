#include "Win32MessageBoxLogger.h"

#include "Environment/Windows/WindowsIncluder.h"

#include "Kernel/DebugBreak.h"
#include "Kernel/ConfigurationHelper.h"

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

        // Honor engine-wide silent-dialog mode without going through any
        // service: the configuration lives on the ServiceProvider itself,
        // so this works even if PlatformService isn't up yet.
        if( Helper::isSilentDialog() == true )
        {
            return;
        }

        LoggerMessage message;
        _record->getMessage( &message );

        ::MessageBoxA( NULL, message.data, "Mengine critical error", MB_OK | MB_ICONERROR );
    }
    //////////////////////////////////////////////////////////////////////////
}
