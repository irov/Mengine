#include "SDL2DynamicLibrary.h"

#include "Environment/SDL2/SDL2Includer.h"

#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"

#include "Config/StdString.h"
#include "Config/StdIO.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SDL2DynamicLibrary::SDL2DynamicLibrary()
        : m_instance( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SDL2DynamicLibrary::~SDL2DynamicLibrary()
    {
        this->unload();
    }
    //////////////////////////////////////////////////////////////////////////
    void SDL2DynamicLibrary::setName( const Char * _name )
    {
        m_name.assign( _name );
    }
    //////////////////////////////////////////////////////////////////////////
    const Char * SDL2DynamicLibrary::getName() const
    {
        const Char * name_str = m_name.c_str();

        return name_str;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDL2DynamicLibrary::load()
    {
        const Char * name_str = this->getName();

        void * instance = SDL_LoadObject( name_str );

        if( instance == nullptr )
        {
            LOGGER_ERROR( "dynamic library '%s' failed load object: %s"
                , this->getName()
                , SDL_GetError()
            );

            return false;
        }

        m_instance = instance;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDL2DynamicLibrary::unload()
    {
        if( m_instance != nullptr )
        {
            SDL_UnloadObject( m_instance );
            m_instance = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    TDynamicLibraryFunction SDL2DynamicLibrary::getSymbol( const Char * _name ) const
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_instance, "dynamic library '%s' not loaded"
            , this->getName()
        );

        void * proc = ::SDL_LoadFunction( m_instance, _name );

        if( proc == nullptr )
        {
            LOGGER_ERROR( "dynamic library '%s' failed get symbol '%s' error: %s"
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
