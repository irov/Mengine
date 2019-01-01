#include "Win32PluginSystem.h"

#include "Win32DynamicLibrary.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"

#include "Kernel/Logger.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( PluginSystem, Mengine::Win32PluginSystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Win32PluginSystem::Win32PluginSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32PluginSystem::~Win32PluginSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32PluginSystem::_initializeService()
    {
        m_factoryDynamicLibraries = new FactoryPool<Win32DynamicLibrary, 8>();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32PluginSystem::_finalizeService()
    {
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryDynamicLibraries );

        m_factoryDynamicLibraries = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    DynamicLibraryInterfacePtr Win32PluginSystem::loadDynamicLibrary( const Char * _dynamicLibraryName )
    {
        LOGGER_INFO( "load dynamic library '%s'"
            , _dynamicLibraryName
        );

        Win32DynamicLibraryPtr dynamicLibrary = m_factoryDynamicLibraries->createObject();

        MENGINE_ASSERTION_MEMORY_PANIC( dynamicLibrary, nullptr );

        dynamicLibrary->setName( _dynamicLibraryName );

        if( dynamicLibrary->load() == false )
        {
            LOGGER_ERROR( "can't load dynamic library '%s' [invalid load]"
                , _dynamicLibraryName
            );

            return nullptr;
        }

        return dynamicLibrary;
    }
}