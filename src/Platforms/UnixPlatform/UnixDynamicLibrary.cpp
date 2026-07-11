#include "UnixDynamicLibrary.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Logger.h"

#include <dlfcn.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    UnixDynamicLibrary::UnixDynamicLibrary()
        : m_instance( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    UnixDynamicLibrary::~UnixDynamicLibrary()
    {
        this->unload();
    }
    //////////////////////////////////////////////////////////////////////////
    void UnixDynamicLibrary::setName( const Char * _name )
    {
        m_name.assign( _name );
    }
    //////////////////////////////////////////////////////////////////////////
    const Char * UnixDynamicLibrary::getName() const
    {
        return m_name.c_str();
    }
    //////////////////////////////////////////////////////////////////////////
    bool UnixDynamicLibrary::load()
    {
        ::dlerror();

        void * instance = ::dlopen( m_name.c_str(), RTLD_NOW | RTLD_LOCAL );

        if( instance == nullptr )
        {
            const Char * error = ::dlerror();

            LOGGER_ERROR( "dynamic library '%s' failed to load: %s"
                , m_name.c_str()
                , error != nullptr ? error : "unknown error"
            );

            return false;
        }

        m_instance = instance;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void UnixDynamicLibrary::unload()
    {
        if( m_instance == nullptr )
        {
            return;
        }

        ::dlclose( m_instance );
        m_instance = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    TDynamicLibraryFunction UnixDynamicLibrary::getSymbol( const Char * _name ) const
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_instance, "dynamic library '%s' is not loaded"
            , m_name.c_str()
        );

        ::dlerror();
        void * symbol = ::dlsym( m_instance, _name );
        const Char * error = ::dlerror();

        if( error != nullptr )
        {
            LOGGER_ERROR( "dynamic library '%s' failed to resolve '%s': %s"
                , m_name.c_str()
                , _name
                , error
            );

            return nullptr;
        }

        return reinterpret_cast<TDynamicLibraryFunction>(symbol);
    }
    //////////////////////////////////////////////////////////////////////////
}
