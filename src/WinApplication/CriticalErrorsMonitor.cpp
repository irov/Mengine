#	include "CriticalErrorsMonitor.h"

#   include "WindowsLayer/WindowsIncluder.h"
#   include "DbgHelp.h"

#	include <cstdio>

#	include "Interface/WindowsLayerInterface.h"

#ifdef _MSC_VER
#	define snprintf _snprintf
#endif

namespace Menge
{
	static WString s_logPath;
    static WString s_dumpPath;
    //////////////////////////////////////////////////////////////////////////
    static bool s_writeCrashDump( EXCEPTION_POINTERS * pExceptionPointers )
    {
        HANDLE hFile = CreateFile( s_dumpPath.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );

        if( hFile == INVALID_HANDLE_VALUE ) 
        {
            return false;
        }
        
        HMODULE dbghelp_dll = ::LoadLibrary(L"dbghelp.dll");

        if( dbghelp_dll == NULL )
        {
            ::CloseHandle( hFile );

            return false;
        }

        typedef BOOL (WINAPI *MINIDUMPWRITEDUMP)(
            HANDLE hprocess, DWORD pid, HANDLE hfile, MINIDUMP_TYPE dumptype,
            CONST PMINIDUMP_EXCEPTION_INFORMATION exceptionparam,
            CONST PMINIDUMP_USER_STREAM_INFORMATION userstreamparam,
            CONST PMINIDUMP_CALLBACK_INFORMATION callbackparam
            );

        MINIDUMPWRITEDUMP MiniDumpWriteDump = (MINIDUMPWRITEDUMP)::GetProcAddress(dbghelp_dll, "MiniDumpWriteDump");

        if( MiniDumpWriteDump == NULL )
        {
            FreeLibrary( dbghelp_dll );
            ::CloseHandle( hFile );

            return false;
        }

        MINIDUMP_EXCEPTION_INFORMATION exinfo;

        exinfo.ThreadId = ::GetCurrentThreadId();
        exinfo.ExceptionPointers = pExceptionPointers;
        exinfo.ClientPointers = FALSE;

        HANDLE hProcess = GetCurrentProcess();
        DWORD dwProcessId = GetCurrentProcessId();
                
        BOOL successful = MiniDumpWriteDump( hProcess, dwProcessId, hFile, MiniDumpNormal, &exinfo, NULL, NULL );

        FreeLibrary( dbghelp_dll );
        ::CloseHandle( hFile );

        if( successful == FALSE )
        {
            return false;
        }

        return true;
    }

	//////////////////////////////////////////////////////////////////////////
	static LONG WINAPI s_exceptionHandler(EXCEPTION_POINTERS* pExceptionPointers)
	{       
        s_writeCrashDump( pExceptionPointers );
                
		return EXCEPTION_EXECUTE_HANDLER;
	}
	//////////////////////////////////////////////////////////////////////////
	void CriticalErrorsMonitor::run( const WString & _logPath, const WString & _dumpPath )
	{
		s_logPath = _logPath;
        s_dumpPath = _dumpPath;

		::SetErrorMode( SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX );
		::SetUnhandledExceptionFilter( &s_exceptionHandler );
	}
}
