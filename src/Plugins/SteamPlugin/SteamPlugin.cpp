#include "SteamPlugin.h"

#include "Interface/OptionsServiceInterface.h"
#include "Interface/NotificationServiceInterface.h"
#include "Interface/ScriptServiceInterface.h"

#ifdef MENGINE_USE_SCRIPT_SERVICE
#include "SteamScriptEmbedding.h"
#endif

#include "Kernel/ModuleFactory.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/ConfigHelper.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/Logger.h"

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
        if( HAS_OPTION( "nosteam" ) == true )
        {
            return false;
        }

        if( HAS_OPTION( "steam" ) == false )
        {
            bool Steam_Avaliable = CONFIG_VALUE( "Steam", "Avaliable", true );

            if( Steam_Avaliable == false )
            {
                return false;
            }
        }

        uint32_t Steam_AppId = CONFIG_VALUE( "Steam", "AppId", k_uAppIdInvalid );

        if( HAS_OPTION( "steamappid" ) == true )
        {
            const Char * str_steamappid = GET_OPTION_VALUE( "steamappid", "" );

            if( MENGINE_SSCANF( str_steamappid, "%u", &Steam_AppId ) != 0 )
            {
                LOGGER_ERROR( "invalid option steamappid '%s'"
                    , str_steamappid
                );

                return false;
            }
        }

        if( HAS_OPTION( "norestartsteamapp" ) == false )
        {
            if( SteamAPI_RestartAppIfNecessary( Steam_AppId ) == true )
            {
                LOGGER_ERROR( "invalid SteamAPI_RestartAppIfNecessary [Id = %u]"
                    , Steam_AppId
                );

                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool SteamPlugin::_initializePlugin()
    {
        SERVICE_CREATE( SteamService, MENGINE_DOCUMENT_FACTORABLE );

#ifdef MENGINE_USE_SCRIPT_SERVICE
        NOTIFICATION_ADDOBSERVERLAMBDA_THIS( NOTIFICATOR_SCRIPT_EMBEDDING, [this]()
        {
            SCRIPT_SERVICE()
                ->addScriptEmbedding( STRINGIZE_STRING_LOCAL( "SteamScriptEmbedding" ), Helper::makeFactorableUnique<SteamScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) );
        }, MENGINE_DOCUMENT_FACTORABLE );

        NOTIFICATION_ADDOBSERVERLAMBDA_THIS( NOTIFICATOR_SCRIPT_EJECTING, []()
        {
            SCRIPT_SERVICE()
                ->removeScriptEmbedding( STRINGIZE_STRING_LOCAL( "SteamScriptEmbedding" ) );
        }, MENGINE_DOCUMENT_FACTORABLE );
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SteamPlugin::_finalizePlugin()
    {
#ifdef MENGINE_USE_SCRIPT_SERVICE
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EMBEDDING );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EJECTING );
#endif

        SERVICE_FINALIZE( SteamService );
    }
    //////////////////////////////////////////////////////////////////////////
    void SteamPlugin::_destroyPlugin()
    {
        SERVICE_DESTROY( SteamService );
    }
    //////////////////////////////////////////////////////////////////////////
}
