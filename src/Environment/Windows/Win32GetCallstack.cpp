#include "Win32GetCallstack.h"

#ifndef MENGINE_PLATFORM_UWP
#   ifndef MENGINE_UNSUPPORT_PRAGMA_WARNING
#       pragma warning(push, 0)
#   endif

#   include "Environment/Windows/DbgHelp.h"

#   ifndef MENGINE_UNSUPPORT_PRAGMA_WARNING
#       pragma warning(pop)
#   endif
#endif

#include "Config/StdString.h"
#include "Config/StdIO.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_PLATFORM_UWP
        //////////////////////////////////////////////////////////////////////////
        namespace Detail
        {
            //////////////////////////////////////////////////////////////////////////
            // dbghelp.dll
            //////////////////////////////////////////////////////////////////////////
            typedef VOID( __stdcall * TRtlCaptureContext )(OUT PCONTEXT ContextRecord);
            typedef BOOL( __stdcall * TSymCleanup )(IN HANDLE hProcess);
            typedef PVOID( __stdcall * TSymFunctionTableAccess64 )(HANDLE hProcess, DWORD64 AddrBase);
            typedef BOOL( __stdcall * TSymGetLineFromAddr64 )(IN HANDLE hProcess, IN DWORD64 dwAddr, OUT PDWORD pdwDisplacement, OUT PIMAGEHLP_LINE64 Line);
            typedef DWORD64( __stdcall * TSymGetModuleBase64 )(IN HANDLE hProcess, IN DWORD64 dwAddr);
            typedef BOOL( __stdcall * TSymGetModuleInfo64 )(IN HANDLE hProcess, IN DWORD64 dwAddr, OUT PIMAGEHLP_MODULE64 ModuleInfo);
            typedef DWORD( __stdcall * TSymGetOptions )(VOID);
            typedef BOOL( __stdcall * TSymGetSymFromAddr64 )(IN HANDLE hProcess, IN DWORD64 dwAddr, OUT PDWORD64 pdwDisplacement, OUT PIMAGEHLP_SYMBOL64 Symbol);
            typedef BOOL( __stdcall * TSymInitialize )(IN HANDLE hProcess, IN PSTR UserSearchPath, IN BOOL fInvadeProcess);
            typedef DWORD64( __stdcall * TSymLoadModule64 )(IN HANDLE hProcess, IN HANDLE hFile, IN PCSTR ImageName, IN PCSTR ModuleName, IN DWORD64 BaseOfDll, IN DWORD SizeOfDll);
            typedef DWORD( __stdcall * TSymSetOptions )(IN DWORD SymOptions);
            typedef BOOL( __stdcall * TStackWalk64 )(
                DWORD MachineType,
                HANDLE hProcess,
                HANDLE hThread,
                LPSTACKFRAME64 StackFrame,
                PVOID ContextRecord,
                PREAD_PROCESS_MEMORY_ROUTINE64 ReadMemoryRoutine,
                PFUNCTION_TABLE_ACCESS_ROUTINE64 FunctionTableAccessRoutine,
                PGET_MODULE_BASE_ROUTINE64 GetModuleBaseRoutine,
                PTRANSLATE_ADDRESS_ROUTINE64 TranslateAddress);
            typedef DWORD( __stdcall WINAPI * TUnDecorateSymbolName )(PCSTR DecoratedName, PSTR UnDecoratedName, DWORD UndecoratedLength, DWORD Flags);
            typedef BOOL( __stdcall WINAPI * TSymGetSearchPath )(HANDLE hProcess, PSTR SearchPath, DWORD SearchPathLength);
            //////////////////////////////////////////////////////////////////////////
            // psapi.dll
            //////////////////////////////////////////////////////////////////////////
            typedef BOOL( __stdcall * TEnumProcessModules )(HANDLE hProcess, HMODULE * lphModule, DWORD cb, LPDWORD lpcbNeeded);
            typedef DWORD( __stdcall * TGetModuleFileNameEx )(HANDLE hProcess, HMODULE hModule, LPSTR lpFilename, DWORD nSize);
            typedef DWORD( __stdcall * TGetModuleBaseName )(HANDLE hProcess, HMODULE hModule, LPSTR lpFilename, DWORD nSize);
            typedef BOOL( __stdcall * TGetModuleInformation )(HANDLE hProcess, HMODULE hModule, LPMODULEINFO pmi, DWORD nSize);
            //////////////////////////////////////////////////////////////////////////
            static BOOL GetModuleListPSAPI( TSymLoadModule64 pSymLoadModule64, HANDLE hProcess )
            {
                HINSTANCE hPsapi = ::LoadLibraryW( L"psapi.dll" );

                if( hPsapi == NULL )
                {
                    return FALSE;
                }

                TEnumProcessModules pEnumProcessModules = (TEnumProcessModules)::GetProcAddress( hPsapi, "EnumProcessModules" );
                TGetModuleFileNameEx pGetModuleFileNameExA = (TGetModuleFileNameEx)::GetProcAddress( hPsapi, "GetModuleFileNameExA" );
                TGetModuleBaseName pGetModuleBaseNameA = (TGetModuleFileNameEx)::GetProcAddress( hPsapi, "GetModuleBaseNameA" );
                TGetModuleInformation pGetModuleInformation = (TGetModuleInformation)::GetProcAddress( hPsapi, "GetModuleInformation" );

                if( (pEnumProcessModules == NULL) || (pGetModuleFileNameExA == NULL) || (pGetModuleBaseNameA == NULL) || (pGetModuleInformation == NULL) )
                {
                    ::FreeLibrary( hPsapi );

                    return FALSE;
                }

                HMODULE hMods[2048];

                DWORD cbNeeded;
                if( (*pEnumProcessModules)(hProcess, hMods, sizeof( hMods ), &cbNeeded) == FALSE )
                {
                    ::FreeLibrary( hPsapi );

                    return FALSE;
                }

                //cppcheck-suppress uninitvar
                if( cbNeeded > 2048 )
                {
                    ::FreeLibrary( hPsapi );

                    return FALSE;
                }

                for( DWORD i = 0; i != cbNeeded / sizeof( HMODULE ); i++ )
                {
                    MODULEINFO mi;
                    if( (*pGetModuleInformation)(hProcess, hMods[i], &mi, sizeof mi) == FALSE )
                    {
                        ::FreeLibrary( hPsapi );

                        return FALSE;
                    }

                    Char mFileName[MAX_PATH] = {'\0'};
                    DWORD FileNameLen = (*pGetModuleFileNameExA)(hProcess, hMods[i], mFileName, MAX_PATH);

                    Char mBaseName[MAX_PATH] = {'\0'};
                    DWORD BaseNameLen = (*pGetModuleBaseNameA)(hProcess, hMods[i], mBaseName, MAX_PATH);

                    if( FileNameLen <= 0 || BaseNameLen <= 0 )
                    {
                        ::FreeLibrary( hPsapi );

                        return FALSE;
                    }

                    if( pSymLoadModule64( hProcess, 0, mFileName, mBaseName, (DWORD64)mi.lpBaseOfDll, mi.SizeOfImage ) == FALSE )
                    {
                        ::FreeLibrary( hPsapi );

                        return FALSE;
                    }
                }

                ::FreeLibrary( hPsapi );

                return TRUE;
            }
            //////////////////////////////////////////////////////////////////////////
            static BOOL GetModuleInfo( TSymGetModuleInfo64 pSymGetModuleInfo64, HANDLE hProcess, DWORD64 baseAddr, PIMAGEHLP_MODULE64 pModuleInfo )
            {
                if( pSymGetModuleInfo64 == NULL )
                {
                    return FALSE;
                }

                pModuleInfo->SizeOfStruct = sizeof( IMAGEHLP_MODULE64 );

                uint8_t pData[4096]; // reserve enough memory, so the bug in v6.3.5.1 does not lead to memory-overwrites...
                MENGINE_MEMCPY( pData, pModuleInfo, sizeof( IMAGEHLP_MODULE64 ) );

                if( pSymGetModuleInfo64( hProcess, baseAddr, (IMAGEHLP_MODULE64 *)pData ) == FALSE )
                {
                    return FALSE;
                }

                MENGINE_MEMCPY( pModuleInfo, pData, sizeof( IMAGEHLP_MODULE64 ) );
                pModuleInfo->SizeOfStruct = sizeof( IMAGEHLP_MODULE64 );

                return TRUE;
            }
            //////////////////////////////////////////////////////////////////////////
            static BOOL __stdcall ReadMemoryRoutine( HANDLE hProcess, DWORD64 qwBaseAddress, PVOID lpBuffer, DWORD nSize, LPDWORD lpNumberOfBytesRead )
            {
                SIZE_T st;
                BOOL bRet = ::ReadProcessMemory( hProcess, (LPVOID)qwBaseAddress, lpBuffer, nSize, &st );

                *lpNumberOfBytesRead = (DWORD)st;

                return bRet;
            }
            //////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_STACKWALK_MAX_NAMELEN
#define MENGINE_STACKWALK_MAX_NAMELEN 1024
#endif
            //////////////////////////////////////////////////////////////////////////
            typedef struct _CallstackEntry
            {
                DWORD64 offset;
                CHAR name[MENGINE_STACKWALK_MAX_NAMELEN];
                CHAR undName[MENGINE_STACKWALK_MAX_NAMELEN];
                CHAR undFullName[MENGINE_STACKWALK_MAX_NAMELEN];
                DWORD lineNumber;
                CHAR lineFileName[MENGINE_STACKWALK_MAX_NAMELEN];
                CHAR moduleName[MENGINE_STACKWALK_MAX_NAMELEN];
            } CallstackEntry;
            //////////////////////////////////////////////////////////////////////////
            enum CallstackEntryType
            {
                firstEntry, nextEntry, lastEntry
            };
            //////////////////////////////////////////////////////////////////////////
            static size_t OnCallstackEntry( Char * const _stack, size_t _capacity, CallstackEntry & entry )
            {
                CHAR buffer[MENGINE_STACKWALK_MAX_NAMELEN];
                if( entry.offset == 0 )
                {
                    return 0;
                }

                if( entry.name[0] == 0 )
                {
                    MENGINE_STRCPY( entry.name, "(function-name not available)" );
                }

                if( entry.undName[0] != 0 )
                {
                    MENGINE_STRCPY( entry.name, entry.undName );
                }

                if( entry.undFullName[0] != 0 )
                {
                    MENGINE_STRCPY( entry.name, entry.undFullName );
                }

                if( entry.lineFileName[0] == 0 )
                {
                    MENGINE_STRCPY( entry.lineFileName, "(filename not available)" );

                    if( entry.moduleName[0] == 0 )
                    {
                        MENGINE_STRCPY( entry.moduleName, "(module-name not available)" );
                    }

                    MENGINE_SNPRINTF( buffer, MENGINE_STACKWALK_MAX_NAMELEN, "%p (%s): %s (%d): %s\n"
                        , (LPVOID)entry.offset
                        , entry.moduleName
                        , entry.lineFileName
                        , entry.lineNumber
                        , entry.name
                    );
                }
                else
                {
                    MENGINE_SNPRINTF( buffer, MENGINE_STACKWALK_MAX_NAMELEN, "%s (%d): %s\n"
                        , entry.lineFileName
                        , entry.lineNumber
                        , entry.name
                    );
                }

                size_t buffer_len = MENGINE_STRLEN( buffer );

                if( _capacity < buffer_len )
                {
                    return 0;
                }

                MENGINE_STRCAT( _stack, buffer );

                return buffer_len;
            }
            //////////////////////////////////////////////////////////////////////////
            static bool GetCallstack( Char * const _stack, size_t _capacity, PCONTEXT _context, HMODULE hDbhHelp, HMODULE hKernel32, HANDLE hThread, HANDLE hProcess )
            {
                if( _capacity == 0 )
                {
                    return true;
                }

                _stack[0] = '\0';
                --_capacity;

                TRtlCaptureContext pRtlCaptureContext = (TRtlCaptureContext)::GetProcAddress( hKernel32, "RtlCaptureContext" );
                TStackWalk64 pStackWalk64 = (TStackWalk64)::GetProcAddress( hDbhHelp, "StackWalk64" );
                TSymGetOptions pSymGetOptions = (TSymGetOptions)::GetProcAddress( hDbhHelp, "SymGetOptions" );
                TSymSetOptions pSymSetOptions = (TSymSetOptions)::GetProcAddress( hDbhHelp, "SymSetOptions" );

                TSymFunctionTableAccess64 pSymFunctionTableAccess64 = (TSymFunctionTableAccess64)::GetProcAddress( hDbhHelp, "SymFunctionTableAccess64" );
                TSymGetLineFromAddr64 pSymGetLineFromAddr64 = (TSymGetLineFromAddr64)::GetProcAddress( hDbhHelp, "SymGetLineFromAddr64" );
                TSymGetModuleBase64 pSymGetModuleBase64 = (TSymGetModuleBase64)::GetProcAddress( hDbhHelp, "SymGetModuleBase64" );
                TSymGetModuleInfo64 pSymGetModuleInfo64 = (TSymGetModuleInfo64)::GetProcAddress( hDbhHelp, "SymGetModuleInfo64" );
                TSymGetSymFromAddr64 pSymGetSymFromAddr64 = (TSymGetSymFromAddr64)::GetProcAddress( hDbhHelp, "SymGetSymFromAddr64" );
                TUnDecorateSymbolName pUnDecorateSymbolName = (TUnDecorateSymbolName)::GetProcAddress( hDbhHelp, "UnDecorateSymbolName" );
                TSymLoadModule64 pSymLoadModule64 = (TSymLoadModule64)::GetProcAddress( hDbhHelp, "SymLoadModule64" );
                TSymGetSearchPath pSymGetSearchPath = (TSymGetSearchPath)::GetProcAddress( hDbhHelp, "SymGetSearchPath" );

                if( pRtlCaptureContext == NULL || pStackWalk64 == NULL || pSymGetOptions == NULL ||
                    pSymSetOptions == NULL || pSymFunctionTableAccess64 == NULL || pSymGetModuleBase64 == NULL ||
                    pSymGetModuleInfo64 == NULL || pSymGetSymFromAddr64 == NULL || pUnDecorateSymbolName == NULL || pSymLoadModule64 == NULL ||
                    pSymGetSearchPath == NULL )
                {
                    return false;
                }

                DWORD symOptions = pSymGetOptions();
                symOptions |= SYMOPT_LOAD_LINES;
                symOptions |= SYMOPT_FAIL_CRITICAL_ERRORS;

                symOptions = (*pSymSetOptions)(symOptions);

                if( Detail::GetModuleListPSAPI( pSymLoadModule64, hProcess ) == FALSE )
                {
                    return false;
                }

                CONTEXT contex;

                if( _context == NULL )
                {
                    MENGINE_MEMSET( &contex, 0, sizeof( CONTEXT ) );
                    contex.ContextFlags = CONTEXT_FULL;

                    HANDLE hCurrentThread = ::GetCurrentThread();

                    if( hThread == hCurrentThread )
                    {
                        (*pRtlCaptureContext)(&contex);
                    }
                    else
                    {
                        if( ::SuspendThread( hThread ) == (DWORD)-1 )
                        {
                            return false;
                        }

                        if( ::GetThreadContext( hThread, &contex ) == FALSE )
                        {
                            ::ResumeThread( hThread );

                            return false;
                        }
                    }
                }
                else
                {
                    contex = *_context;
                }

                STACKFRAME64 frame;
                MENGINE_MEMSET( &frame, 0, sizeof( frame ) );

#if defined(MENGINE_MACHINE_IX86)
                DWORD imageType = IMAGE_FILE_MACHINE_I386;
                frame.AddrPC.Offset = contex.Eip;
                frame.AddrPC.Mode = AddrModeFlat;
                frame.AddrFrame.Offset = contex.Ebp;
                frame.AddrFrame.Mode = AddrModeFlat;
                frame.AddrStack.Offset = contex.Esp;
                frame.AddrStack.Mode = AddrModeFlat;
#elif defined(MENGINE_MACHINE_X64)
                DWORD imageType = IMAGE_FILE_MACHINE_AMD64;
                frame.AddrPC.Offset = contex.Rip;
                frame.AddrPC.Mode = AddrModeFlat;
                frame.AddrFrame.Offset = contex.Rsp;
                frame.AddrFrame.Mode = AddrModeFlat;
                frame.AddrStack.Offset = contex.Rsp;
                frame.AddrStack.Mode = AddrModeFlat;
#elif defined(MENGINE_MACHINE_IA64)
                DWORD imageType = IMAGE_FILE_MACHINE_IA64;
                frame.AddrPC.Offset = contex.StIIP;
                frame.AddrPC.Mode = AddrModeFlat;
                frame.AddrFrame.Offset = contex.IntSp;
                frame.AddrFrame.Mode = AddrModeFlat;
                frame.AddrBStore.Offset = contex.RsBSP;
                frame.AddrBStore.Mode = AddrModeFlat;
                frame.AddrStack.Offset = contex.IntSp;
                frame.AddrStack.Mode = AddrModeFlat;
#else
#   error "Platform not supported!"
#endif

                uint8_t pSymBuff[sizeof( IMAGEHLP_SYMBOL64 ) + MENGINE_STACKWALK_MAX_NAMELEN];
                IMAGEHLP_SYMBOL64 * pSym = (IMAGEHLP_SYMBOL64 *)pSymBuff;
                MENGINE_MEMSET( pSym, 0, sizeof( IMAGEHLP_SYMBOL64 ) + MENGINE_STACKWALK_MAX_NAMELEN );

                pSym->SizeOfStruct = sizeof( IMAGEHLP_SYMBOL64 );
                pSym->MaxNameLength = MENGINE_STACKWALK_MAX_NAMELEN;

                IMAGEHLP_LINE64 Line;
                MENGINE_MEMSET( &Line, 0, sizeof( Line ) );
                Line.SizeOfStruct = sizeof( Line );

                IMAGEHLP_MODULE64 Module;
                MENGINE_MEMSET( &Module, 0, sizeof( Module ) );
                Module.SizeOfStruct = sizeof( Module );

                uint32_t recursionFrame = 0;

                for( int32_t frameNum = 0;; ++frameNum )
                {
                    if( (*pStackWalk64)(imageType, hProcess, hThread, &frame, &contex, &ReadMemoryRoutine, pSymFunctionTableAccess64, pSymGetModuleBase64, NULL) == FALSE )
                    {
                        if( _context == NULL )
                        {
                            ::ResumeThread( hThread );
                        }

                        return false;
                    }

                    if( frame.AddrPC.Offset == frame.AddrReturn.Offset )
                    {
                        if( ++recursionFrame == 1024 )
                        {
                            break;
                        }
                    }

                    CallstackEntry csEntry;
                    MENGINE_MEMSET( &csEntry, 0, sizeof( csEntry ) );
                    csEntry.offset = frame.AddrPC.Offset;

                    if( frame.AddrPC.Offset != 0 )
                    {
                        DWORD64 offsetFromSmybol;
                        if( (*pSymGetSymFromAddr64)(hProcess, frame.AddrPC.Offset, &offsetFromSmybol, pSym) == TRUE )
                        {
                            MENGINE_STRCPY( csEntry.name, pSym->Name );
                            (*pUnDecorateSymbolName)(pSym->Name, csEntry.undName, MENGINE_STACKWALK_MAX_NAMELEN, UNDNAME_NAME_ONLY);
                            (*pUnDecorateSymbolName)(pSym->Name, csEntry.undFullName, MENGINE_STACKWALK_MAX_NAMELEN, UNDNAME_COMPLETE);
                        }

                        if( pSymGetLineFromAddr64 != NULL )
                        {
                            DWORD offsetFromLine;
                            if( (*pSymGetLineFromAddr64)(hProcess, frame.AddrPC.Offset, &offsetFromLine, &Line) == TRUE )
                            {
                                csEntry.lineNumber = Line.LineNumber;
                                MENGINE_STRCPY( csEntry.lineFileName, Line.FileName );
                            }
                        }

                        if( Detail::GetModuleInfo( pSymGetModuleInfo64, hProcess, frame.AddrPC.Offset, &Module ) == TRUE )
                        {
                            MENGINE_STRCPY( csEntry.moduleName, Module.ModuleName );
                        }
                    }

                    size_t stack_len = Detail::OnCallstackEntry( _stack, _capacity, csEntry );

                    _capacity -= stack_len;

                    if( frame.AddrReturn.Offset == 0 )
                    {
                        break;
                    }
                }

                if( _context == NULL )
                {
                    ::ResumeThread( hThread );
                }

                return true;
            }
            //////////////////////////////////////////////////////////////////////////
        }
        //////////////////////////////////////////////////////////////////////////
        bool Win32GetCallstack( DWORD _threadId, PCONTEXT _context, Char * const _stack, size_t _capacity )
        {
            HANDLE hProcess = ::GetCurrentProcess();

            HMODULE hDbhHelp = ::LoadLibraryW( L"dbghelp.dll" );

            if( hDbhHelp == NULL )
            {
                return false;
            }

            Detail::TSymInitialize pSymInitialize = (Detail::TSymInitialize)::GetProcAddress( hDbhHelp, "SymInitialize" );
            Detail::TSymCleanup pSymCleanup = (Detail::TSymCleanup)::GetProcAddress( hDbhHelp, "SymCleanup" );

            if( pSymInitialize == NULL || pSymCleanup == NULL )
            {
                ::FreeLibrary( hDbhHelp );

                return false;
            }

            HMODULE hKernel32 = ::LoadLibraryW( L"Kernel32.dll" );

            if( hKernel32 == NULL )
            {
                ::FreeLibrary( hDbhHelp );

                return false;
            }

            if( (*pSymInitialize)(hProcess, NULL, FALSE) == FALSE )
            {
                ::FreeLibrary( hDbhHelp );
                ::FreeLibrary( hKernel32 );

                return false;
            }

            HANDLE hThread;

            if( _threadId == ~0U )
            {
                hThread = ::GetCurrentThread();
            }
            else
            {
                hThread = ::OpenThread( THREAD_GET_CONTEXT | THREAD_SUSPEND_RESUME | THREAD_QUERY_INFORMATION, FALSE, _threadId );
            }

            if( hThread == NULL )
            {
                ::FreeLibrary( hDbhHelp );
                ::FreeLibrary( hKernel32 );

                return false;
            }

            bool successful = Detail::GetCallstack( _stack, _capacity, (PCONTEXT)_context, hDbhHelp, hKernel32, hThread, hProcess );

            (*pSymCleanup)(hProcess);

            if( _threadId != ~0U )
            {
                ::CloseHandle( hThread );
            }

            ::FreeLibrary( hDbhHelp );
            ::FreeLibrary( hKernel32 );

            return successful;
        }
        //////////////////////////////////////////////////////////////////////////
#else
        //////////////////////////////////////////////////////////////////////////
        bool Win32GetCallstack( DWORD _threadId, PCONTEXT _context, Char * const _stack, size_t _capacity )
        {
            MENGINE_UNUSED( _threadId );
            MENGINE_UNUSED( _context );
            MENGINE_UNUSED( _stack );
            MENGINE_UNUSED( _capacity );

            return false;
        }
        //////////////////////////////////////////////////////////////////////////
#endif
    }
}