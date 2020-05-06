#include "SDLDynamicLibrary.h"

#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"

#include "Config/StdString.h"

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
    void SDLDynamicLibrary::setName( const Char * _name )
    {
        MENGINE_STRCPY( m_name, _name );
    }
    //////////////////////////////////////////////////////////////////////////
    const Char * SDLDynamicLibrary::getName() const
    {
        return m_name;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLDynamicLibrary::load()
    {
        void * instance = SDL_LoadObject( m_name );

        if( instance == nullptr )
        {
            LOGGER_ERROR( "'%s' failed"
                , m_name
            );

            return false;
        }

        m_instance = instance;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    TDynamicLibraryFunction SDLDynamicLibrary::getSymbol( const Char * _name ) const
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_instance, nullptr );

        void * proc = ::SDL_LoadFunction( m_instance, _name );

        TDynamicLibraryFunction dlfunc = reinterpret_cast<TDynamicLibraryFunction>(proc);

        return dlfunc;
    }
    //////////////////////////////////////////////////////////////////////////
}
