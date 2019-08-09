#include "Win32CriticalErrorsMonitor.h"

#include "Interface/UnicodeSystemInterface.h"
#include "Interface/PlatformInterface.h"

#include "Kernel/Logger.h"

#include "Environment/Windows/WindowsIncluder.h"

#include "stdex/stack.h"

#include <cstdio>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    struct CrashDumpExceptionHandlerData
    {
        String dumpPath;
    };
    //////////////////////////////////////////////////////////////////////////
    static CrashDumpExceptionHandlerData * g_crashDumpExceptionHandlerData = nullptr;
    //////////////////////////////////////////////////////////////////////////
    static bool s_writeCrashDump( EXCEPTION_POINTERS * pExceptionPointers )
    {
        if( g_crashDumpExceptionHandlerData == nullptr )
        {
            return false;
        }

        if( PLATFORM_SERVICE()
            ->createProcessDump( g_crashDumpExceptionHandlerData->dumpPath.c_str(), pExceptionPointers, false ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    static LONG WINAPI s_exceptionHandler( EXCEPTION_POINTERS * pExceptionPointers )
    {
        LOGGER_ERROR( "Exception catch" );

        std::string stack;
        stdex::get_callstack( stack, pExceptionPointers->ContextRecord );

        LOGGER_CRITICAL( "catch exception and write dumb '%s'\n\n\n %s\n\n\n"
            , g_crashDumpExceptionHandlerData->dumpPath.c_str()
            , stack.c_str()
        );

        s_writeCrashDump( pExceptionPointers );

        return EXCEPTION_EXECUTE_HANDLER;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32CriticalErrorsMonitor::run( const Char * _dumpPath )
    {
        g_crashDumpExceptionHandlerData = new CrashDumpExceptionHandlerData;

        g_crashDumpExceptionHandlerData->dumpPath = _dumpPath;

        ::SetErrorMode( SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX );
        ::SetUnhandledExceptionFilter( &s_exceptionHandler );
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32CriticalErrorsMonitor::stop()
    {
        if( g_crashDumpExceptionHandlerData != nullptr )
        {
            delete g_crashDumpExceptionHandlerData;
            g_crashDumpExceptionHandlerData = nullptr;
        }
    }
}
