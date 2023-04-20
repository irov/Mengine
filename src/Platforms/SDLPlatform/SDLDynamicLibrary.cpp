#include "SDLDynamicLibrary.h"

#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"

#include "Config/StdString.h"
#include "Config/StdIO.h"

#include "SDL_loadso.h"
#include "SDL_filesystem.h"

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
        this->unload();
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLDynamicLibrary::setName( const Char * _name )
    {
        m_name.assign( _name );
    }
    //////////////////////////////////////////////////////////////////////////
    const Char * SDLDynamicLibrary::getName() const
    {
        const Char * name_str = m_name.c_str();

        return name_str;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLDynamicLibrary::load()
    {
        const Char * name_str = this->getName();

        void * instance = SDL_LoadObject( name_str );

        if( instance == nullptr )
        {
            LOGGER_ERROR( "'%s' failed load object: %s"
                , this->getName()
                , SDL_GetError()
            );

            return false;
        }

        m_instance = instance;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLDynamicLibrary::unload()
    {
        if( m_instance != nullptr )
        {
            SDL_UnloadObject( m_instance );
            m_instance = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    TDynamicLibraryFunction SDLDynamicLibrary::getSymbol( const Char * _name ) const
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_instance );

        void * proc = ::SDL_LoadFunction( m_instance, _name );

        if( proc == nullptr )
        {
            LOGGER_ERROR( "'%s' failed get symbol '%s': %s"
                , this->getName()
                , _name
                , SDL_GetError()
            );

            return nullptr;
        }

        TDynamicLibraryFunction dlfunc = reinterpret_cast<TDynamicLibraryFunction>(proc);

        return dlfunc;
    }
    //////////////////////////////////////////////////////////////////////////
}
