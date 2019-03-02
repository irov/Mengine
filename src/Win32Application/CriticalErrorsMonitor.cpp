#include "CriticalErrorsMonitor.h"

#include "Interface/UnicodeSystemInterface.h"

#include "Kernel/Date.h"

#include "Kernel/Logger.h"

#include "stdex/stack.h"

#include "Environment/Windows/WindowsIncluder.h"

#ifndef MENGINE_UNSUPPORT_PRAGMA_WARNING
#	pragma warning(push, 0) 
#endif

#include "DbgHelp.h"

#ifndef MENGINE_UNSUPPORT_PRAGMA_WARNING
#	pragma warning(pop) 
#endif

#include <cstdio>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    struct CrashDumpExceptionHandlerData
    {
        WString dumpPath;
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

        HANDLE hFile = CreateFile( g_crashDumpExceptionHandlerData->dumpPath.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0 );

        if( hFile == INVALID_HANDLE_VALUE )
        {
            return false;
        }

        HMODULE dbghelp_dll = ::LoadLibrary( L"dbghelp.dll" );

        if( dbghelp_dll == NULL )
        {
            ::CloseHandle( hFile );

            return false;
        }

        typedef BOOL( WINAPI *MINIDUMPWRITEDUMP )(
            HANDLE hprocess, DWORD pid, HANDLE hfile, MINIDUMP_TYPE dumptype,
            CONST PMINIDUMP_EXCEPTION_INFORMATION exceptionparam,
            CONST PMINIDUMP_USER_STREAM_INFORMATION userstreamparam,
            CONST PMINIDUMP_CALLBACK_INFORMATION callbackparam
            );

        MINIDUMPWRITEDUMP MiniDumpWriteDump = (MINIDUMPWRITEDUMP)::GetProcAddress( dbghelp_dll, "MiniDumpWriteDump" );

        if( MiniDumpWriteDump == NULL )
        {
            FreeLibrary( dbghelp_dll );
            ::CloseHandle( hFile );

            return false;
        }

        MINIDUMP_EXCEPTION_INFORMATION exinfo;

        exinfo.ThreadId = ::GetCurrentThreadId();
        exinfo.ExceptionPointers = pExceptionPointers;
        exinfo.ClientPointers = TRUE;

        HANDLE hProcess = GetCurrentProcess();
        DWORD dwProcessId = GetCurrentProcessId();

        MINIDUMP_TYPE dumptype = MINIDUMP_TYPE( MiniDumpNormal | MiniDumpWithIndirectlyReferencedMemory | MiniDumpWithDataSegs | MiniDumpWithThreadInfo );

        BOOL successful = MiniDumpWriteDump( hProcess, dwProcessId, hFile, dumptype, &exinfo, NULL, NULL );

        FreeLibrary( dbghelp_dll );
        ::CloseHandle( hFile );

        if( successful == FALSE )
        {
            return false;
        }

        return true;
    }

    //////////////////////////////////////////////////////////////////////////
    static LONG WINAPI s_exceptionHandler( EXCEPTION_POINTERS* pExceptionPointers )
    {
        s_writeCrashDump( pExceptionPointers );

        std::string stack;
        stdex::get_callstack( stack, pExceptionPointers->ContextRecord );

        LOGGER_CRITICAL( "catch exception and write dumb %ls\n\n\n %s\n\n\n"
            , g_crashDumpExceptionHandlerData->dumpPath.c_str()
            , stack.c_str()
        );

        return EXCEPTION_EXECUTE_HANDLER;
    }
    //////////////////////////////////////////////////////////////////////////
    void CriticalErrorsMonitor::run( const Char * _userPath )
    {
        String dumpPath;
        dumpPath += _userPath;
        dumpPath += "Dump";
        dumpPath += "_";

        String date;
        Helper::makeDateTime( date );

        dumpPath += date;
        dumpPath += ".dmp";

        g_crashDumpExceptionHandlerData = new CrashDumpExceptionHandlerData;

        Helper::utf8ToUnicode( dumpPath, g_crashDumpExceptionHandlerData->dumpPath );

        ::SetErrorMode( SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX );
        ::SetUnhandledExceptionFilter( &s_exceptionHandler );
    }
    //////////////////////////////////////////////////////////////////////////
    void CriticalErrorsMonitor::stop()
    {
        if( g_crashDumpExceptionHandlerData != nullptr )
        {
            delete g_crashDumpExceptionHandlerData;
            g_crashDumpExceptionHandlerData = nullptr;
        }
    }
}
