#   include "SDLPluginSystem.h"

#   include "SDLDynamicLibrary.h"

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
    DynamicLibraryInterface * SDLPluginSystem::loadDynamicLibrary( const WString & _dynamicLibraryName )
    {
        LOGGER_WARNING( m_serviceProvider )("SDLPluginSystem::loadDynamicLibrary plugin '%ls'"
            , _dynamicLibraryName.c_str()
            );

        Char utf8Path[1025] = { 0 };
        size_t utf8Size = 0;
        if (!UNICODE_SERVICE(m_serviceProvider)->unicodeToUtf8(_dynamicLibraryName.c_str(), _dynamicLibraryName.size(),
                                                               utf8Path, sizeof(utf8Path) - 1, &utf8Size))
        {
            LOGGER_ERROR(m_serviceProvider)("SDLPluginSystem::loadDynamicLibrary can't get utf8 path");
            return nullptr;
        }


        SDLDynamicLibrary * dynamicLibrary = new SDLDynamicLibrary(m_serviceProvider, utf8Path);

        if (!dynamicLibrary->load())
        {
            LOGGER_ERROR( m_serviceProvider )("SDLPluginSystem::loadDynamicLibrary can't load '%ls' plugin [invalid load]"
                , _dynamicLibraryName.c_str()
                );

            dynamicLibrary->destroy();

            return nullptr;
        }

        return dynamicLibrary;
    }
}
