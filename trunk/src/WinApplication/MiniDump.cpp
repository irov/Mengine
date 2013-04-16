#   include "MiniDump.h"

#   include <windows.h>
#   include <stdio.h>
#   include <shlobj.h>

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    MiniDump::MiniDump()
    {
        m_hfile = INVALID_HANDLE_VALUE;
        m_hdll  = NULL;
    }
    //////////////////////////////////////////////////////////////////////////
    MiniDump::~MiniDump()
    {
        WString fullpath[MAX_PATH] = {0};
        wyString fullpathstr, appnamestr;

        if(m_hfile != INVALID_HANDLE_VALUE)
        {
            if(SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, fullpath)))
                fullpathstr.SetAs(fullpath);

            appnamestr.SetAs(m_appname);
            fullpathstr.AddSprintf("\\SQLyog\\%s", appnamestr.GetString());

            CloseHandle(m_hfile);
            DeleteFile(fullpathstr.GetAsWideChar());
        }

        if(m_hdll)
            FreeLibrary(m_hdll);
    }
    //////////////////////////////////////////////////////////////////////////

    //////////////////////////////////////////////////////////////////////////
    static LONG WINAPI s_exceptionFilter( EXCEPTION_POINTERS *pexceptioninfo )
    {
        m_hfile = ::CreateFile(fullpathstr.GetAsWideChar(), GENERIC_WRITE, FILE_SHARE_WRITE, NULL, 
            CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
    }

    //////////////////////////////////////////////////////////////////////////
    void MiniDump::initialize( const WString & _filePath )
    {
        wchar_t dbghelppath[_MAX_PATH];
        wchar_t * pslash;

        CreateUniqueDumpFile();

        ::SetUnhandledExceptionFilter( &s_exceptionFilter );

        m_dbghelp = ::LoadLibrary(L"dbghelp.dll");
        
        if( m_dbghelp == NULL )
        {
            MiniDumpWriteDump = (MINIDUMPWRITEDUMP)::GetProcAddress(m_hdll, "MiniDumpWriteDump");
        }

        return;
    }

    LONG 
        MiniDump::HandleSQLyogCrashDump(struct _EXCEPTION_POINTERS *pexceptioninfo)
    {
        LONG        retval = EXCEPTION_CONTINUE_SEARCH;
        wyWChar     msg[512] = {0}, apppath[MAX_PATH];
        BOOL        ok;
        wyString    dumppath, appnamestr;

        if(m_hfile == INVALID_HANDLE_VALUE || !m_hdll || !m_dump)
        {
            MessageBox(NULL, DBGHELP, TEXT(MSGCAPTION), MB_ICONERROR);
            return retval;
        }

        _MINIDUMP_EXCEPTION_INFORMATION exinfo;

        exinfo.ThreadId = ::GetCurrentThreadId();
        exinfo.ExceptionPointers = pexceptioninfo;
        exinfo.ClientPointers = NULL;

        // write the dump
        ok = m_dump(GetCurrentProcess(), GetCurrentProcessId(), m_hfile, 
            MiniDumpNormal, &exinfo, NULL, NULL );

        if(ok)
        {
            if(SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, apppath)))
            {
                dumppath.SetAs(apppath);
                appnamestr.SetAs(m_appname);
                dumppath.AddSprintf("\\SQLyog\\%s", appnamestr.GetString());
            }
            _snwprintf(msg, 511, L"%s%s", TEXT(CRASH_MSG), dumppath.GetAsWideChar());
            MessageBox(NULL, msg, TEXT(MSGCAPTION), MB_ICONERROR);
            retval  = EXCEPTION_EXECUTE_HANDLER;
        }
        else
            MessageBox(NULL, TEXT(FAIL_MSG), TEXT(MSGCAPTION), MB_ICONERROR);

        ::CloseHandle(m_hfile);
        m_hfile = INVALID_HANDLE_VALUE;

        return retval;
    }

    void MiniDump::CreateUniqueDumpFile()
    {
        wyWChar     basename[] = L"_Dump_", filename[32], fullpath[MAX_PATH + 1] = {0};
        wyInt32     count = 0;
        wyString    filenamestr, fullpathstr, apppathstr;

        //GetModuleFileName(NULL, apppath, MAX_PATH);
        if(SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, fullpath)))
        {
            fullpathstr.SetAs(fullpath);
            fullpathstr.Add("\\SQLyog");
        }
        else
            return;

        /*apppathstr.SetAs(apppath);
        directory = strrchr(apppathstr.GetString(), '\\');

        if(!directory)
        return;

        *(directory+1) = '\0';*/

        for(count = 0; count < 999; count++)
        {

            _snwprintf(filename, 31, L"%s%s%03d.dmp", TEXT(DUMPNAME), basename, count);
            filenamestr.SetAs(filename);
            fullpathstr.SetAs(fullpath);
            fullpathstr.AddSprintf("\\SQLyog\\%s", filenamestr.GetString());

            m_hfile = ::CreateFile(fullpathstr.GetAsWideChar(), GENERIC_WRITE, FILE_SHARE_WRITE, NULL, 
                CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);

            if(m_hfile != INVALID_HANDLE_VALUE)
            {
                wcscpy(m_appname, filename);
                break;
            }
        }

        //All the 1000 dump files are existing force a filecreation
        if(m_hfile == INVALID_HANDLE_VALUE)
        {
            count = 0;
            _snwprintf(filename, 31, L"%s%s%3d", DUMPNAME, basename, count);
            filenamestr.SetAs(filename);
            fullpathstr.Add(filenamestr.GetString());
            m_hfile = ::CreateFile(fullpathstr.GetAsWideChar(), GENERIC_WRITE, FILE_SHARE_WRITE, NULL, 
                CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

            if(m_hfile != INVALID_HANDLE_VALUE)
                wcscpy(m_appname, filename);
        }
        return;
    }
}
