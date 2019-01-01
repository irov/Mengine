#include "SDLPluginSystem.h"

#include "SDLDynamicLibrary.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Logger.h"

#include "Interface/UnicodeSystemInterface.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( PluginSystem, Mengine::SDLPluginSystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SDLPluginSystem::SDLPluginSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SDLPluginSystem::~SDLPluginSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool SDLPluginSystem::_initializeService()
    {
        m_factoryDynamicLibraries = new FactoryPool<SDLDynamicLibrary, 8>();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLPluginSystem::_finalizeService()
    {
        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryDynamicLibraries );

        m_factoryDynamicLibraries = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    DynamicLibraryInterfacePtr SDLPluginSystem::loadDynamicLibrary( const Char * _dynamicLibraryName )
    {
        LOGGER_INFO( "load dynamic library '%s'"
            , _dynamicLibraryName
        );

        SDLDynamicLibraryPtr dynamicLibrary = m_factoryDynamicLibraries->createObject();

        MENGINE_ASSERTION_MEMORY_PANIC( dynamicLibrary, nullptr )("can't create dynamic library '%s'"
            , _dynamicLibraryName
            );

        dynamicLibrary->setName( _dynamicLibraryName );

        if( dynamicLibrary->load() == false )
        {
            LOGGER_ERROR( "can't load '%s' plugin [invalid load]"
                , _dynamicLibraryName
            );

            return nullptr;
        }

        return dynamicLibrary;
    }
}
