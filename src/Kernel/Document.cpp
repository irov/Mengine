#include "Document.h"

#include "Kernel/MemoryAllocator.h"

#include "Config/StdIO.h"

//////////////////////////////////////////////////////////////////////////
#ifdef MENGINE_PLATFORM_WINDOWS
#   include "Environment/Windows/WindowsIncluder.h"
#endif
//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_DOCUMENT_MAX_MESSAGE
#define MENGINE_DOCUMENT_MAX_MESSAGE 8192
#endif
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Document::Document( const Char * _file, const Char * _function, uint32_t _line )
        : m_file( _file )
        , m_function( _function )
        , m_line( _line )
    {
    }
    /////////////////////////////////////////////////////////////////////////
    Document::~Document()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    const Char * Document::getModulePath() const
    {
        return m_modulePath;
    }
    //////////////////////////////////////////////////////////////////////////
    const Char * Document::getFile() const
    {
        return m_file;
    }
    //////////////////////////////////////////////////////////////////////////
    const Char * Document::getFunction() const
    {
        return m_function;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Document::getLine() const
    {
        return m_line;
    }
    //////////////////////////////////////////////////////////////////////////
    void Document::setParent( const DocumentPtr & _parent )
    {
        m_parent = _parent;
    }
    //////////////////////////////////////////////////////////////////////////
    const DocumentPtr & Document::getParent() const
    {
        return m_parent;
    }
    //////////////////////////////////////////////////////////////////////////
    void Document::message( const char * _format, ... )
    {
#ifdef MENGINE_PLATFORM_WINDOWS
        WCHAR unicode_path[MENGINE_MAX_PATH];
        HMODULE hm = NULL;

        if( GetModuleHandleEx( GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
            GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            L"dllCreatePlugin", &hm ) == 0 )
        {
            int ret = GetLastError();
            fprintf( stderr, "GetModuleHandle failed, error = %d\n", ret );
            // Return or however you want to handle an error.
        }
        if( GetModuleFileName( hm, unicode_path, sizeof( unicode_path ) ) == 0 )
        {
            int ret = GetLastError();
            fprintf( stderr, "GetModuleFileName failed, error = %d\n", ret );
            // Return or however you want to handle an error.
        }

#   if (WINVER >= 0x0600)
        DWORD dwConversionFlags = WC_ERR_INVALID_CHARS;
#   else
        DWORD dwConversionFlags = 0;
#   endif

        ::WideCharToMultiByte(
            CP_UTF8
            , dwConversionFlags
            , unicode_path
            , wcslen( unicode_path )
            , m_modulePath
            , MENGINE_MAX_PATH
            , NULL
            , NULL
        );
#endif

        MENGINE_VA_LIST_TYPE args;
        MENGINE_VA_LIST_START( args, _format );

        Char message[MENGINE_DOCUMENT_MAX_MESSAGE] = {0};
        int message_size = MENGINE_VSNPRINTF( message, MENGINE_DOCUMENT_MAX_MESSAGE, _format, args );

        MENGINE_VA_LIST_END( args );

        m_message.assign( message, message_size );
    }
    //////////////////////////////////////////////////////////////////////////
    const Char * Document::c_str() const
    {
        return m_message.c_str();
    }
}