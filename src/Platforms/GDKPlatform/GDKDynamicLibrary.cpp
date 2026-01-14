#include "GDKDynamicLibrary.h"

#include "Environment/Windows/Win32Helper.h"

#include "Kernel/Logger.h"
#include "Kernel/UnicodeHelper.h"

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    GDKDynamicLibrary::GDKDynamicLibrary()
        : m_hInstance( NULL )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    GDKDynamicLibrary::~GDKDynamicLibrary()
    {
        this->unload();
    }
    //////////////////////////////////////////////////////////////////////////
    void GDKDynamicLibrary::setName( const Char * _name )
    {
        m_name.assign( _name );
    }
    //////////////////////////////////////////////////////////////////////////
    const Char * GDKDynamicLibrary::getName() const
    {
        return m_name.c_str();
    }
    //////////////////////////////////////////////////////////////////////////
    bool GDKDynamicLibrary::load()
    {
        if( m_hInstance != NULL )
        {
            return true;
        }

        WChar libraryPathW[MENGINE_MAX_PATH] = {L'\0'};
        if( Helper::utf8ToUnicode( m_name.c_str(), libraryPathW, MENGINE_MAX_PATH, nullptr ) == false )
        {
            return false;
        }

        m_hInstance = ::LoadLibraryW( libraryPathW );

        if( m_hInstance == NULL )
        {
            LOGGER_ERROR( "invalid load library '%s' error %ls"
                , m_name.c_str()
                , Helper::Win32GetLastErrorMessageW()
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void GDKDynamicLibrary::unload()
    {
        if( m_hInstance != NULL )
        {
            ::FreeLibrary( m_hInstance );
            m_hInstance = NULL;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    TDynamicLibraryFunction GDKDynamicLibrary::getSymbol( const Char * _name ) const
    {
        if( m_hInstance == NULL )
        {
            LOGGER_ERROR( "library '%s' not loaded"
                , m_name.c_str()
            );

            return nullptr;
        }

        TDynamicLibraryFunction proc = (TDynamicLibraryFunction)::GetProcAddress( m_hInstance, _name );

        if( proc == nullptr )
        {
            LOGGER_ERROR( "library '%s' invalid get symbol '%s' error %ls"
                , m_name.c_str()
                , _name
                , Helper::Win32GetLastErrorMessageW()
            );

            return nullptr;
        }

        return proc;
    }
    //////////////////////////////////////////////////////////////////////////
}
