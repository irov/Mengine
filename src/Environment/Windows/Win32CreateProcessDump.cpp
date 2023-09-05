#include "Win32CreateProcessDump.h"

#ifndef MENGINE_UNSUPPORT_PRAGMA_WARNING
#   pragma warning(push, 0) 
#endif

#include "Environment/Windows/DbgHelp.h"

#ifndef MENGINE_UNSUPPORT_PRAGMA_WARNING
#   pragma warning(pop) 
#endif

#include "Environment/Windows/Win32Helper.h"

#include "Kernel/UnicodeHelper.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    namespace Helper
    {
        bool Win32CreateProcessDump( const Char * _dumpPath, PEXCEPTION_POINTERS _pExceptionPointers, bool _fullDump )
        {
            WChar unicode_processDumpPath[MENGINE_MAX_PATH] = {L'\0'};
            Helper::utf8ToUnicode( _dumpPath, unicode_processDumpPath, MENGINE_MAX_PATH );

            HANDLE hFile = ::CreateFile( unicode_processDumpPath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0 );

            if( hFile == INVALID_HANDLE_VALUE )
            {
                LOGGER_ERROR( "invalid create file for '%ls' %ls"
                    , unicode_processDumpPath
                    , Helper::Win32GetLastErrorMessage()
                );

                return false;
            }

            HMODULE dbghelp_dll = ::LoadLibrary( L"dbghelp.dll" );

            if( dbghelp_dll == NULL )
            {
                ::CloseHandle( hFile );

                return false;
            }

            typedef BOOL(WINAPI * MINIDUMPWRITEDUMP)(
                HANDLE hprocess, DWORD pid, HANDLE hfile, MINIDUMP_TYPE dumptype,
                CONST PMINIDUMP_EXCEPTION_INFORMATION exceptionparam,
                CONST PMINIDUMP_USER_STREAM_INFORMATION userstreamparam,
                CONST PMINIDUMP_CALLBACK_INFORMATION callbackparam
                );

            MINIDUMPWRITEDUMP MiniDumpWriteDump = (MINIDUMPWRITEDUMP)::GetProcAddress( dbghelp_dll, "MiniDumpWriteDump" );

            if( MiniDumpWriteDump == NULL )
            {
                ::FreeLibrary( dbghelp_dll );
                ::CloseHandle( hFile );

                return false;
            }

            MINIDUMP_EXCEPTION_INFORMATION exInfo;

            exInfo.ThreadId = ::GetCurrentThreadId();
            exInfo.ExceptionPointers = _pExceptionPointers;
            exInfo.ClientPointers = TRUE;

            HANDLE hProcess = ::GetCurrentProcess();
            DWORD dwProcessId = ::GetCurrentProcessId();

            MINIDUMP_TYPE dumptype;

            if( _fullDump == false )
            {
                dumptype = MINIDUMP_TYPE( MiniDumpNormal | MiniDumpWithIndirectlyReferencedMemory | MiniDumpWithDataSegs | MiniDumpWithThreadInfo );
            }
            else
            {
                dumptype = MINIDUMP_TYPE( MiniDumpWithFullMemory | MiniDumpWithFullMemoryInfo | MiniDumpWithHandleData | MiniDumpWithUnloadedModules | MiniDumpWithThreadInfo );
            }

            BOOL successful = (*MiniDumpWriteDump)(hProcess, dwProcessId, hFile, dumptype, (_pExceptionPointers == nullptr ? nullptr : &exInfo), NULL, NULL);

            ::FreeLibrary( dbghelp_dll );
            ::CloseHandle( hFile );

            if( successful == FALSE )
            {
                return false;
            }

            return true;
        }
    }
}