#include "AppleGameCenterPlugin.h"
#include "AppleGameCenterInterface.h"

#include "Interface/NotificationServiceInterface.h"
#include "Interface/ScriptServiceInterface.h"

#ifdef MENGINE_USE_SCRIPT_SERVICE
#include "AppleGameCenterScriptEmbedding.h"
#endif

#include "Kernel/ConstStringHelper.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( Amplifier );
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( Amplifier, Mengine::AppleGameCenterPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleGameCenterPlugin::AppleGameCenterPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleGameCenterPlugin::~AppleGameCenterPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleGameCenterPlugin::_initializePlugin()
    {
        SERVICE_CREATE( AppleGameCenter, MENGINE_DOCUMENT_FACTORABLE );

#ifdef MENGINE_USE_SCRIPT_SERVICE
        NOTIFICATION_ADDOBSERVERLAMBDA_THIS( NOTIFICATOR_SCRIPT_EMBEDDING, [this]()
        {
            SCRIPT_SERVICE()
                ->addScriptEmbedding( STRINGIZE_STRING_LOCAL( "AppleGameCenterScriptEmbedding" ), Helper::makeFactorableUnique<AppleGameCenterScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) );
        }, MENGINE_DOCUMENT_FACTORABLE );

        NOTIFICATION_ADDOBSERVERLAMBDA_THIS( NOTIFICATOR_SCRIPT_EJECTING, []()
        {
            SCRIPT_SERVICE()
                ->removeScriptEmbedding( STRINGIZE_STRING_LOCAL( "AppleGameCenterScriptEmbedding" ) );
        }, MENGINE_DOCUMENT_FACTORABLE );
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleGameCenterPlugin::_finalizePlugin()
    {
#ifdef MENGINE_USE_SCRIPT_SERVICE
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EMBEDDING );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EJECTING );
#endif

        SERVICE_FINALIZE( AppleGameCenter );
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleGameCenterPlugin::_destroyPlugin()
    {
        SERVICE_DESTROY( AppleGameCenter );
    }
    //////////////////////////////////////////////////////////////////////////
}