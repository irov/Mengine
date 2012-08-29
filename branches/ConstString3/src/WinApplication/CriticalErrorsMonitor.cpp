#	include "CriticalErrorsMonitor.h"

#   include <Windows.h>
#   include "DbgHelp.h"

#	include <cstdio>

#	include "WindowsLayer/WindowsLayer.h"

#ifdef _MSC_VER
#	define snprintf _snprintf
#endif

namespace Menge
{
	static const char * s_versionInfo = NULL;
	static WString s_logPath;
    static WString s_dumpPath;
	//////////////////////////////////////////////////////////////////////////
	static void s_logStackFrames( HANDLE _hFile, void* _faultAddress, char* eNextBP )
	{
		char wBuffer[4096];
		DWORD wr;
		char *p, *pBP;                                     
		unsigned i, x, BpPassed;
		static int  CurrentlyInTheStackDump = 0;
		//...
		BpPassed = (eNextBP != NULL);
		if(! eNextBP)
		{
#if defined _MSC_VER
			_asm mov     eNextBP, eBp   
#else
			asm("mov  %ebp, 12(%ebp);");
#endif
		}
		else 
		{
			sprintf( wBuffer, "\n  Fault Occurred At $ADDRESS:%p\n", _faultAddress );
			::WriteFile( _hFile, wBuffer, strlen( wBuffer ), &wr, 0 );
		}
		// prevent infinite loops
		for(i = 0; eNextBP && i < 100; i++)
		{      
			pBP = eNextBP;           // keep current BasePointer
			eNextBP = *(char **)pBP; // dereference next BP 
			p = pBP + 8; 
			// Write 20 Bytes of potential arguments
			sprintf( wBuffer, "         with " );
			::WriteFile( _hFile, wBuffer, strlen( wBuffer ), &wr, 0 );
			for(x = 0; p < eNextBP && x < 20; p++, x++)
			{
				sprintf( wBuffer, "%02X ", *(unsigned char *)p );
				::WriteFile( _hFile, wBuffer, strlen( wBuffer ), &wr, 0 );
			}
			sprintf( wBuffer, "\n\n" );
			::WriteFile( _hFile, wBuffer, strlen( wBuffer ), &wr, 0 );
			if(i == 1 && ! BpPassed) 
			{
				sprintf( wBuffer, "Fault Occurred Here:\n" );
				::WriteFile( _hFile, wBuffer, strlen( wBuffer ), &wr, 0 );
			}
			// Write the backjump address
			sprintf( wBuffer, "*** %2d called from $ADDRESS:%p\n", i, *(char **)(pBP + 4) );
			::WriteFile( _hFile, wBuffer, strlen( wBuffer ), &wr, 0 );
			if(*(char **)(pBP + 4) == NULL)
			{
				break; 
			}
		}

		CurrentlyInTheStackDump = 0;
	}
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
		//EXCEPTION_RECORD* pRecord = pExceptionPointers->ExceptionRecord;
		//CONTEXT* pContext = pExceptionPointers->ContextRecord;

		//HANDLE hFile = WindowsLayer::createFile( s_logPath.c_str(), GENERIC_READ|GENERIC_WRITE, 
		//	FILE_SHARE_WRITE|FILE_SHARE_READ, OPEN_ALWAYS );

		//if( hFile != INVALID_HANDLE_VALUE )
		//{
		//	DWORD wr;
		//	SYSTEMTIME tm;
		//	GetLocalTime(&tm);

		//	OSVERSIONINFOA os_ver;
		//	os_ver.dwOSVersionInfoSize = sizeof(os_ver);
		//	GetVersionExA(&os_ver);

		//	char wBuffer[4096];
		//	::SetFilePointer( hFile, 0, 0, FILE_END );

		//	strcpy( wBuffer, "\n=============Unhandled Exception Caugth=============\n" );
		//	::WriteFile( hFile, wBuffer, strlen( wBuffer ),&wr, 0 );
		//	snprintf( wBuffer, 4096, "Date: %02d.%02d.%d, %02d:%02d:%02d\n", tm.wDay, tm.wMonth, tm.wYear, tm.wHour, tm.wMinute, tm.wSecond );
		//	::WriteFile( hFile, wBuffer, strlen( wBuffer ),&wr, 0 );
		//	snprintf( wBuffer, 4096, "OS: Windows %ld.%ld.%ld\n", os_ver.dwMajorVersion, os_ver.dwMinorVersion, os_ver.dwBuildNumber );
		//	::WriteFile( hFile, wBuffer, strlen( wBuffer ),&wr, 0 );
		//	snprintf( wBuffer, 4096, "Source SVN Revision: %s", s_versionInfo );
		//	::WriteFile( hFile, wBuffer, strlen( wBuffer ),&wr, 0 );
		//	strcpy( wBuffer, "\nCrash Info:\n" );
		//	::WriteFile( hFile, wBuffer, strlen( wBuffer ), &wr, 0 );
		//	snprintf( wBuffer, 4096, "Exception Code: 0x%08x\n", pRecord->ExceptionCode );
		//	::WriteFile( hFile, wBuffer, strlen( wBuffer ), &wr, 0 );
		//	snprintf( wBuffer, 4096, "Flags: 0x%08x\n", pRecord->ExceptionFlags );
		//	::WriteFile( hFile, wBuffer, strlen( wBuffer ), &wr, 0 );
		//	snprintf( wBuffer, 4096, "Address: %p\n\n", pRecord->ExceptionAddress );
		//	::WriteFile( hFile, wBuffer, strlen( wBuffer ), &wr, 0 );
		//	if( ( pContext->ContextFlags & CONTEXT_INTEGER ) != 0 )
		//	{
		//		snprintf( wBuffer, 4096, "Edi: 0x%08x\t Esi: 0x%08x\n", pContext->Edi, pContext->Esi );
		//		::WriteFile( hFile, wBuffer, strlen( wBuffer ), &wr, 0 );
		//		snprintf( wBuffer, 4096, "Ebx: 0x%08x\t Edx: 0x%08x\n", pContext->Ebx, pContext->Edx );
		//		::WriteFile( hFile, wBuffer, strlen( wBuffer ), &wr, 0 );
		//		snprintf( wBuffer, 4096, "Ecx: 0x%08x\t Eax: 0x%08x\n\n", pContext->Ecx, pContext->Eax );
		//		::WriteFile( hFile, wBuffer, strlen( wBuffer ), &wr, 0 );
		//	}
		//	if( ( pContext->ContextFlags & CONTEXT_CONTROL ) != 0 )
		//	{
		//		snprintf( wBuffer, 4096, "Ebp: 0x%08x\t Eip: 0x%08x\n", pContext->Ebp, pContext->Eip );
		//		::WriteFile( hFile, wBuffer, strlen( wBuffer ), &wr, 0 );
		//		snprintf( wBuffer, 4096, "SegCs: 0x%08x\t EFlags: 0x%08x\n", pContext->SegCs, pContext->EFlags );
		//		::WriteFile( hFile, wBuffer, strlen( wBuffer ), &wr, 0 );
		//		snprintf( wBuffer, 4096, "Esp: 0x%08x\t SegSs: 0x%08x\n", pContext->Esp, pContext->SegSs );
		//		::WriteFile( hFile, wBuffer, strlen( wBuffer ), &wr, 0 );
		//	}

		//	s_logStackFrames( hFile, pRecord->ExceptionAddress, (char*)pContext->Ebp );
		//	/*switch (pRecord->ExceptionCode) 
		//	{
		//	case EXCEPTION_ACCESS_VIOLATION:
		//	case EXCEPTION_IN_PAGE_ERROR:
		//	case EXCEPTION_INT_DIVIDE_BY_ZERO:
		//	case EXCEPTION_STACK_OVERFLOW:
		//	}*/

		//	::CloseHandle( hFile );
		//}
        
        s_writeCrashDump( pExceptionPointers );
                
		return EXCEPTION_EXECUTE_HANDLER;
	}
	//////////////////////////////////////////////////////////////////////////
	void CriticalErrorsMonitor::run( const char * _versionInfo, const WString & _logPath, const WString & _dumpPath )
	{
		s_versionInfo = _versionInfo;
		s_logPath = _logPath;
        s_dumpPath = _dumpPath;

		::SetErrorMode( SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX );
		::SetUnhandledExceptionFilter( &s_exceptionHandler );
	}
}
