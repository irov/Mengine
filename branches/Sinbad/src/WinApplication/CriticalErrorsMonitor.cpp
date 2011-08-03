#	include "CriticalErrorsMonitor.h"

#	include <cstdio>

#ifdef _MSC_VER
#	define snprintf _snprintf
#endif

namespace Menge
{
	static const char * s_versionInfo = NULL;
	static const WCHAR * s_userPath = NULL;
	static const WCHAR * s_logFileName = NULL;
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
			sprintf( wBuffer, "\n  Fault Occurred At $ADDRESS:%08LX\n", _faultAddress );
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
			sprintf( wBuffer, "*** %2d called from $ADDRESS:%08LX\n", i, *(char **)(pBP + 4) );
			::WriteFile( _hFile, wBuffer, strlen( wBuffer ), &wr, 0 );
			if(*(char **)(pBP + 4) == NULL)
			{
				break; 
			}
		}

		CurrentlyInTheStackDump = 0;
	}
	//////////////////////////////////////////////////////////////////////////
	static LONG WINAPI s_exceptionHandler(EXCEPTION_POINTERS* pExceptionPointers)
	{
		EXCEPTION_RECORD* pRecord = pExceptionPointers->ExceptionRecord;
		CONTEXT* pContext = pExceptionPointers->ContextRecord;

		WCHAR fullFileName[MAX_PATH];
		wcsncpy( fullFileName, s_userPath, MAX_PATH );
		wcsncat( fullFileName, s_logFileName, MAX_PATH );

		HANDLE hFile = ::CreateFile( fullFileName, GENERIC_READ|GENERIC_WRITE, 
			FILE_SHARE_WRITE|FILE_SHARE_READ, 0, OPEN_ALWAYS, 0, 0 );

		if( hFile != INVALID_HANDLE_VALUE )
		{
			DWORD wr;
			SYSTEMTIME tm;
			GetLocalTime(&tm);

			OSVERSIONINFO os_ver;
			os_ver.dwOSVersionInfoSize = sizeof(os_ver);
			GetVersionEx(&os_ver);

			char wBuffer[4096];
			::SetFilePointer( hFile, 0, 0, FILE_END );

			strcpy( wBuffer, "\n=============Unhandled Exception Caugth=============\n" );
			::WriteFile( hFile, wBuffer, strlen( wBuffer ),&wr, 0 );
			snprintf( wBuffer, 4096, "Date: %02d.%02d.%d, %02d:%02d:%02d\n", tm.wDay, tm.wMonth, tm.wYear, tm.wHour, tm.wMinute, tm.wSecond );
			::WriteFile( hFile, wBuffer, strlen( wBuffer ),&wr, 0 );
			snprintf( wBuffer, 4096, "OS: Windows %ld.%ld.%ld\n", os_ver.dwMajorVersion, os_ver.dwMinorVersion, os_ver.dwBuildNumber );
			::WriteFile( hFile, wBuffer, strlen( wBuffer ),&wr, 0 );
			snprintf( wBuffer, 4096, "Source SVN Revision: %s", s_versionInfo );
			::WriteFile( hFile, wBuffer, strlen( wBuffer ),&wr, 0 );
			strcpy( wBuffer, "\nCrash Info:\n" );
			::WriteFile( hFile, wBuffer, strlen( wBuffer ), &wr, 0 );
			snprintf( wBuffer, 4096, "Exception Code: 0x%08x\n", pRecord->ExceptionCode );
			::WriteFile( hFile, wBuffer, strlen( wBuffer ), &wr, 0 );
			snprintf( wBuffer, 4096, "Flags: 0x%08x\n", pRecord->ExceptionFlags );
			::WriteFile( hFile, wBuffer, strlen( wBuffer ), &wr, 0 );
			snprintf( wBuffer, 4096, "Address: 0x%08x\n\n", pRecord->ExceptionAddress );
			::WriteFile( hFile, wBuffer, strlen( wBuffer ), &wr, 0 );
			if( ( pContext->ContextFlags & CONTEXT_INTEGER ) != 0 )
			{
				snprintf( wBuffer, 4096, "Edi: 0x%08x\t Esi: 0x%08x\n", pContext->Edi, pContext->Esi );
				::WriteFile( hFile, wBuffer, strlen( wBuffer ), &wr, 0 );
				snprintf( wBuffer, 4096, "Ebx: 0x%08x\t Edx: 0x%08x\n", pContext->Ebx, pContext->Edx );
				::WriteFile( hFile, wBuffer, strlen( wBuffer ), &wr, 0 );
				snprintf( wBuffer, 4096, "Ecx: 0x%08x\t Eax: 0x%08x\n\n", pContext->Ecx, pContext->Eax );
				::WriteFile( hFile, wBuffer, strlen( wBuffer ), &wr, 0 );
			}
			if( ( pContext->ContextFlags & CONTEXT_CONTROL ) != 0 )
			{
				snprintf( wBuffer, 4096, "Ebp: 0x%08x\t Eip: 0x%08x\n", pContext->Ebp, pContext->Eip );
				::WriteFile( hFile, wBuffer, strlen( wBuffer ), &wr, 0 );
				snprintf( wBuffer, 4096, "SegCs: 0x%08x\t EFlags: 0x%08x\n", pContext->SegCs, pContext->EFlags );
				::WriteFile( hFile, wBuffer, strlen( wBuffer ), &wr, 0 );
				snprintf( wBuffer, 4096, "Esp: 0x%08x\t SegSs: 0x%08x\n", pContext->Esp, pContext->SegSs );
				::WriteFile( hFile, wBuffer, strlen( wBuffer ), &wr, 0 );
			}
			s_logStackFrames( hFile, pRecord->ExceptionAddress, (char*)pContext->Ebp );
			/*switch (pRecord->ExceptionCode) 
			{
			case EXCEPTION_ACCESS_VIOLATION:
			case EXCEPTION_IN_PAGE_ERROR:
			case EXCEPTION_INT_DIVIDE_BY_ZERO:
			case EXCEPTION_STACK_OVERFLOW:
			}*/

			::CloseHandle( hFile );
		}
		return EXCEPTION_EXECUTE_HANDLER;
	}
	//////////////////////////////////////////////////////////////////////////
	void CriticalErrorsMonitor::run( const char * _versionInfo, const WCHAR * _userPath, const WCHAR * _logFileName )
	{
		s_versionInfo = _versionInfo;
		s_userPath = _userPath;
		s_logFileName = _logFileName;

		::SetErrorMode( SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX );
		::SetUnhandledExceptionFilter( &s_exceptionHandler );
	}
}
