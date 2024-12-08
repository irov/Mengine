#include "AndroidDynamicLibrary.h"

#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"

#include "Config/StdString.h"
#include "Config/StdIO.h"

#include <dlfcn.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AndroidDynamicLibrary::AndroidDynamicLibrary()
        : m_instance( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidDynamicLibrary::~AndroidDynamicLibrary()
    {
        this->unload();
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidDynamicLibrary::setName( const Char * _name )
    {
        m_name.assign( _name );
    }
    //////////////////////////////////////////////////////////////////////////
    const Char * AndroidDynamicLibrary::getName() const
    {
        const Char * name_str = m_name.c_str();

        return name_str;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidDynamicLibrary::load()
    {
        const Char * name_str = this->getName();

        void * instance = ::dlopen( name_str, RTLD_NOW );

        if( instance == nullptr )
        {
            LOGGER_ERROR( "dynamic library '%s' failed load object: %s"
                , this->getName()
                , ::dlerror()
            );

            return false;
        }

        m_instance = instance;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidDynamicLibrary::unload()
    {
        if( m_instance != nullptr )
        {
            ::dlclose( m_instance );
            m_instance = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    TDynamicLibraryFunction AndroidDynamicLibrary::getSymbol( const Char * _name ) const
    {
        MENGINE_ASSERTION_MEMORY_PANIC( m_instance, "dynamic library '%s' not loaded"
            , this->getName()
        );

        void * proc = ::dlsym( m_instance, _name );

        if( proc == nullptr )
        {
            LOGGER_ERROR( "dynamic library '%s' failed get symbol '%s' error: %s"
                , this->getName()
                , _name
                , ::dlerror()
            );

            return nullptr;
        }

        TDynamicLibraryFunction dlfunc = reinterpret_cast<TDynamicLibraryFunction>(proc);

        return dlfunc;
    }
    //////////////////////////////////////////////////////////////////////////
}
