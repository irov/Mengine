#include "SDLPluginSystem.h"

#include "SDLDynamicLibrary.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/FactoryAssertion.h"
#include "Kernel/Logger.h"

#include "Interface/UnicodeInterface.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( PluginSystem, Mengine::SDLPluginSystem);
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
    DynamicLibraryInterfacePtr SDLPluginSystem::loadDynamicLibrary( const WString & _dynamicLibraryName )
    {
        LOGGER_WARNING("SDLPluginSystem::loadDynamicLibrary plugin '%ls'"
            , _dynamicLibraryName.c_str()
            );

        SDLDynamicLibraryPtr dynamicLibrary = m_factoryDynamicLibraries->createObject();

        if( dynamicLibrary == nullptr )
        {
            LOGGER_ERROR("SDLPluginSystem::loadDynamicLibrary can't create dynamic library"
                );

            return nullptr;
        }

        String utf8_dynamicLibraryName;
        if( Helper::unicodeToUtf8( _dynamicLibraryName, utf8_dynamicLibraryName ) == false )
        {
            LOGGER_ERROR( "SDLPluginSystem::loadDynamicLibrary can't get utf8 path"
                );

            return nullptr;
        }
                
        dynamicLibrary->setName( utf8_dynamicLibraryName );

        if( dynamicLibrary->load() == false )
        {
            LOGGER_ERROR( "SDLPluginSystem::loadDynamicLibrary can't load '%ls' plugin [invalid load]"
                , _dynamicLibraryName.c_str()
                );

            return nullptr;
        }

        return dynamicLibrary;
    }
}
