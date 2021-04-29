#include "Win32DynamicLibrary.h"

#include "Interface/PlatformInterface.h"
#include "Interface/Win32PlatformExtensionInterface.h"

#include "Kernel/UnicodeHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/Win32Helper.h"

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
        MENGINE_STRCPY( m_name, _name );
    }
    //////////////////////////////////////////////////////////////////////////
    const Char * Win32DynamicLibrary::getName() const
    {
        return m_name;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32DynamicLibrary::load()
    {
        WChar unicode_name[MENGINE_MAX_PATH] = {L'\0'};
        if( Helper::utf8ToUnicode( m_name, unicode_name, MENGINE_MAX_PATH ) == false )
        {
            return false;
        }

        HINSTANCE hInstance = ::LoadLibrary( unicode_name );

        if( hInstance == NULL )
        {
            LOGGER_ERROR( "invalid load '%s' %s"
                , m_name
                , Helper::Win32GetLastErrorMessage()
            );

            return false;
        }

        WCHAR dllFilename[MENGINE_MAX_PATH] = {L'\0'};
        ::GetModuleFileName( hInstance, dllFilename, MENGINE_MAX_PATH );

        LOGGER_MESSAGE_RELEASE( "load dll: %ls"
            , dllFilename
        );

        m_hInstance = hInstance;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32DynamicLibrary::unload()
    {
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

        TDynamicLibraryFunction dlfunc = (TDynamicLibraryFunction)proc;

        return dlfunc;
    }
    //////////////////////////////////////////////////////////////////////////
}
