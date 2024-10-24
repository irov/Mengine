#include "AppleGeneralDataProtectionRegulationPlugin.h"

#if defined(MENGINE_USE_SCRIPT_SERVICE)
#   include "Interface/ScriptServiceInterface.h"

#   include "AppleGeneralDataProtectionRegulationScriptEmbedding.h"
#endif

#include "AppleGeneralDataProtectionRegulationInterface.h"

#include "Kernel/ConfigHelper.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/NotificationHelper.h"
#include "Kernel/PluginHelper.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( AppleGeneralDataProtectionRegulationService );
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( AppleGeneralDataProtectionRegulation, Mengine::AppleGeneralDataProtectionRegulationPlugin )
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleGeneralDataProtectionRegulationPlugin::AppleGeneralDataProtectionRegulationPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleGeneralDataProtectionRegulationPlugin::~AppleGeneralDataProtectionRegulationPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleGeneralDataProtectionRegulationPlugin::_availablePlugin() const
    {
        if( HAS_OPTION( "applegeneraldataprotectionregulation" ) == true )
        {
            return true;
        }
            
        if( HAS_OPTION( "noapplegeneraldataprotectionregulation" ) == true )
        {
            return false;
        }
        
        bool AppleGeneralDataProtectionRegulationPlugin_Available = CONFIG_VALUE( "AppleGeneralDataProtectionRegulationPlugin", "Available", true );
        
        if( AppleGeneralDataProtectionRegulationPlugin_Available == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleGeneralDataProtectionRegulationPlugin::_initializePlugin()
    {
        if( SERVICE_CREATE( AppleGeneralDataProtectionRegulationService, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }
        
#if defined(MENGINE_USE_SCRIPT_SERVICE)
        NOTIFICATION_ADDOBSERVERLAMBDA_THIS( NOTIFICATOR_SCRIPT_EMBEDDING, [this]()
        {
            SCRIPT_SERVICE()
                ->addScriptEmbedding( STRINGIZE_STRING_LOCAL( "AppleGeneralDataProtectionRegulationScriptEmbedding" ), Helper::makeFactorableUnique<AppleGeneralDataProtectionRegulationScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) );
        }, MENGINE_DOCUMENT_FACTORABLE );

        NOTIFICATION_ADDOBSERVERLAMBDA_THIS( NOTIFICATOR_SCRIPT_EJECTING, []()
        {
            SCRIPT_SERVICE()
                ->removeScriptEmbedding( STRINGIZE_STRING_LOCAL( "AppleGeneralDataProtectionRegulationScriptEmbedding" ) );
        }, MENGINE_DOCUMENT_FACTORABLE );
#endif
        
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleGeneralDataProtectionRegulationPlugin::_finalizePlugin()
    {
#if defined(MENGINE_USE_SCRIPT_SERVICE)
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EMBEDDING );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EJECTING );
#endif
        
        SERVICE_FINALIZE( AppleGeneralDataProtectionRegulationService );
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleGeneralDataProtectionRegulationPlugin::_destroyPlugin()
    {
        SERVICE_DESTROY( AppleGeneralDataProtectionRegulationService );
    }
    //////////////////////////////////////////////////////////////////////////
}
