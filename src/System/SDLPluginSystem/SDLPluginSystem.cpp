#   include "SDLPluginSystem.h"

#   include "SDLDynamicLibrary.h"

#   include "Factory/FactoryPool.h"

#   include "Logger/Logger.h"

#   include "Interface/UnicodeInterface.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( PluginSystem, Menge::SDLPluginSystem);
//////////////////////////////////////////////////////////////////////////
namespace Menge
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
    bool SDLPluginSystem::_initialize()
    {
        m_factoryDynamicLibraries = new FactoryPool<SDLDynamicLibrary, 8>( m_serviceProvider );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SDLPluginSystem::_finalize()
    {
        m_factoryDynamicLibraries = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    DynamicLibraryInterfacePtr SDLPluginSystem::loadDynamicLibrary( const WString & _dynamicLibraryName )
    {
        LOGGER_WARNING( m_serviceProvider )("SDLPluginSystem::loadDynamicLibrary plugin '%ls'"
            , _dynamicLibraryName.c_str()
            );

        SDLDynamicLibraryPtr dynamicLibrary = m_factoryDynamicLibraries->createObject();

        if( dynamicLibrary == nullptr )
        {
            LOGGER_ERROR( m_serviceProvider )("SDLPluginSystem::loadDynamicLibrary can't create dynamic library"
                );

            return nullptr;
        }

        dynamicLibrary->setServiceProvider( m_serviceProvider );

        String utf8_dynamicLibraryName;
        if( Helper::unicodeToUtf8( m_serviceProvider, _dynamicLibraryName, utf8_dynamicLibraryName ) == false )
        {
            LOGGER_ERROR( m_serviceProvider )("SDLPluginSystem::loadDynamicLibrary can't get utf8 path"
                );

            return nullptr;
        }
                
        dynamicLibrary->setName( utf8_dynamicLibraryName );

        if( dynamicLibrary->load() == false )
        {
            LOGGER_ERROR( m_serviceProvider )("SDLPluginSystem::loadDynamicLibrary can't load '%ls' plugin [invalid load]"
                , _dynamicLibraryName.c_str()
                );

            return nullptr;
        }

        return dynamicLibrary;
    }
}
