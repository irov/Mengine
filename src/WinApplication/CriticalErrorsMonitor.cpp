#	include "CriticalErrorsMonitor.h"

#	include "Core/Date.h"

#	include "Logger/Logger.h"

#	include "stdex/stack.h"

#   include "WIN32/WindowsIncluder.h"
#   include "DbgHelp.h"

#	include <cstdio>

#	include "Interface/WindowsLayerInterface.h"

#ifdef _MSC_VER
#	define snprintf _snprintf
#endif

namespace Menge
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
        exinfo.ClientPointers = TRUE;

        HANDLE hProcess = GetCurrentProcess();
        DWORD dwProcessId = GetCurrentProcessId();

		MINIDUMP_TYPE dumptype = MINIDUMP_TYPE(MiniDumpNormal | MiniDumpWithIndirectlyReferencedMemory | MiniDumpWithDataSegs | MiniDumpWithThreadInfo);
        
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

        LOGGER_CRITICAL( "CriticalErrorsMonitor: catch exception and write dumb %ls\n\n\n %s\n\n\n"
			, g_crashDumpExceptionHandlerData->dumpPath.c_str()
			, stack.c_str()
			);
                
		return EXCEPTION_EXECUTE_HANDLER;
	}
	//////////////////////////////////////////////////////////////////////////
	void CriticalErrorsMonitor::run( const WString & _userPath )
	{
		WString dumpPath;
		dumpPath += _userPath;
		dumpPath += L"Dump";
		dumpPath += L"_";

		WString date;
		Helper::makeDateTimeW( date );
		
		dumpPath += date;
		dumpPath += L".dmp";

		g_crashDumpExceptionHandlerData = new CrashDumpExceptionHandlerData;
		g_crashDumpExceptionHandlerData->dumpPath = dumpPath;

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
