#include "Win32DynamicLibrary.h"

#include "Kernel/UnicodeHelper.h"
#include "Kernel/Logger.h"

#include "Config/Config.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Win32DynamicLibrary::Win32DynamicLibrary()
        : m_hInstance( NULL )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32DynamicLibrary::~Win32DynamicLibrary()
    {
        if( m_hInstance != NULL )
        {
            ::FreeLibrary( m_hInstance );
            m_hInstance = NULL;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32DynamicLibrary::setName( const Char * _name )
    {
        strcpy( m_name, _name );
    }
    //////////////////////////////////////////////////////////////////////////
    const Char * Win32DynamicLibrary::getName() const
    {
        return m_name;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32DynamicLibrary::load()
    {
        WChar unicode_name[MENGINE_MAX_PATH];
        if( Helper::utf8ToUnicode( m_name, unicode_name, MENGINE_MAX_PATH ) == false )
        {
            return false;
        }

        HINSTANCE hInstance = ::LoadLibrary( unicode_name );

        if( hInstance == NULL )
        {
            DWORD le = GetLastError();

            LOGGER_ERROR( "invalid load '%s' error code %d"
                , m_name
                , le
            );

            return false;
        }

        m_hInstance = hInstance;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    TDynamicLibraryFunction Win32DynamicLibrary::getSymbol( const Char * _name ) const
    {
        if( m_hInstance == NULL )
        {
            return NULL;
        }

        FARPROC proc = ::GetProcAddress( m_hInstance, _name );

        TDynamicLibraryFunction dlfunc = (TDynamicLibraryFunction)proc;

        return dlfunc;
    }
    //////////////////////////////////////////////////////////////////////////
}
