#include "Win32DynamicLibrary.h"

#include "Environment/Windows/Win32Helper.h"

#include "Kernel/UnicodeHelper.h"
#include "Kernel/Logger.h"

#include "Config/StdString.h"

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
        this->unload();
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32DynamicLibrary::setName( const Char * _name )
    {
        m_name.assign( _name );
    }
    //////////////////////////////////////////////////////////////////////////
    const Char * Win32DynamicLibrary::getName() const
    {
        return m_name.c_str();
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32DynamicLibrary::load()
    {
        LOGGER_INFO( "platform", "load dll: %s"
            , this->getName()
        );

        const Char * name = this->getName();

        WChar unicode_name[MENGINE_MAX_PATH + 1] = {L'\0'};
        if( Helper::utf8ToUnicode( name, unicode_name, MENGINE_MAX_PATH ) == false )
        {
            return false;
        }

        HINSTANCE hInstance = ::LoadLibrary( unicode_name );

        if( hInstance == NULL )
        {
            LOGGER_ERROR( "invalid load '%ls' %ls"
                , unicode_name
                , Helper::Win32GetLastErrorMessageW()
            );

            return false;
        }

        WCHAR dllFilename[MENGINE_MAX_PATH + 1] = {L'\0'};
        ::GetModuleFileName( hInstance, dllFilename, MENGINE_MAX_PATH );

        m_hInstance = hInstance;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32DynamicLibrary::unload()
    {
        LOGGER_INFO( "platform", "unload dll: %s"
            , this->getName()
        );

        if( m_hInstance != NULL )
        {
            ::FreeLibrary( m_hInstance );
            m_hInstance = NULL;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    TDynamicLibraryFunction Win32DynamicLibrary::getSymbol( const Char * _name ) const
    {
        if( m_hInstance == NULL )
        {
            return NULL;
        }

        FARPROC proc = ::GetProcAddress( m_hInstance, _name );

        if( proc == NULL )
        {
            LOGGER_ERROR( "GetProcAddress [%s] get error %ls"
                , _name
                , Helper::Win32GetLastErrorMessageW()
            );

            return NULL;
        }

        TDynamicLibraryFunction dlfunc = (TDynamicLibraryFunction)proc;

        return dlfunc;
    }
    //////////////////////////////////////////////////////////////////////////
}
