#include "SDLDynamicLibrary.h"

#include "Kernel/Logger.h"

#include "SDL_loadso.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SDLDynamicLibrary::SDLDynamicLibrary()
        : m_instance( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SDLDynamicLibrary::~SDLDynamicLibrary()
    {
        if( m_instance != nullptr )
        {
            SDL_UnloadObject( m_instance );
            m_instance = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLDynamicLibrary::setName( const String & _name )
    {
        m_name = _name;
    }
    //////////////////////////////////////////////////////////////////////////
    const String & SDLDynamicLibrary::getName() const
    {
        return m_name;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLDynamicLibrary::load()
    {
        m_instance = SDL_LoadObject( m_name.c_str() );

        if( m_instance == nullptr )
        {
            LOGGER_ERROR( "'%ls' failed"
                , m_name.c_str()
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    TDynamicLibraryFunction SDLDynamicLibrary::getSymbol( const Char * _name ) const
    {
        if( m_instance == nullptr )
        {
            return nullptr;
        }

        void * proc = ::SDL_LoadFunction( m_instance, _name );

        TDynamicLibraryFunction dlfunc = reinterpret_cast<TDynamicLibraryFunction>(proc);

        return dlfunc;
    }
    //////////////////////////////////////////////////////////////////////////
}
