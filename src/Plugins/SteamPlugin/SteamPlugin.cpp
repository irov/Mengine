#include "SteamPlugin.h"

#include "Kernel/ModuleFactory.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/ConfigHelper.h"
#include "Kernel/Error.h"
#include "Kernel/Logger.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/NotificationHelper.h"

#include "Config/StdIO.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( SteamService );
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( Steam, Mengine::SteamPlugin )
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    SteamPlugin::SteamPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    SteamPlugin::~SteamPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool SteamPlugin::_availablePlugin() const
    {
        if( HAS_OPTION( "steam" ) == true )
        {
            return true;
        }

        if( HAS_OPTION( "nosteam" ) == true )
        {
            return false;
        }

        bool SteamPlugin_Available = CONFIG_VALUE( "SteamPlugin", "Available", true );

        if( SteamPlugin_Available == false )
        {
            return false;
        }

        uint32_t SteamPlugin_AppId = CONFIG_VALUE( "SteamPlugin", "AppId", k_uAppIdInvalid );

        if( HAS_OPTION( "steamappid" ) == true )
        {
            const Char * str_steamappid = GET_OPTION_VALUE( "steamappid", "" );

            if( MENGINE_SSCANF( str_steamappid, "%u", &SteamPlugin_AppId ) != 0 )
            {
                LOGGER_ERROR( "invalid option steamappid '%s'"
                    , str_steamappid
                );

                return false;
            }
        }

        if( SteamPlugin_AppId == k_uAppIdInvalid )
        {
            MENGINE_ERROR_FATAL( "invalid setup Steam_AppId" );

            return false;
        }

        if( HAS_OPTION( "norestartsteamapp" ) == false )
        {
            if( SteamAPI_RestartAppIfNecessary( SteamPlugin_AppId ) == true )
            {
                LOGGER_ERROR( "invalid SteamAPI_RestartAppIfNecessary [Id = %u]"
                    , SteamPlugin_AppId
                );

                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SteamPlugin::_initializePlugin()
    {
        if( SERVICE_CREATE( SteamService, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SteamPlugin::_finalizePlugin()
    {
        SERVICE_FINALIZE( SteamService );
    }
    //////////////////////////////////////////////////////////////////////////
    void SteamPlugin::_destroyPlugin()
    {
        SERVICE_DESTROY( SteamService );
    }
    //////////////////////////////////////////////////////////////////////////
}
