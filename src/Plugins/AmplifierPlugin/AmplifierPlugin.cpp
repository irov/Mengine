#include "AmplifierPlugin.h"
#include "AmplifierInterface.h"

#include "Interface/NotificationServiceInterface.h"
#include "Interface/ScriptServiceInterface.h"

#ifdef MENGINE_USE_SCRIPT_SERVICE
#include "AmplifierScriptEmbedding.h"
#endif

#include "Kernel/ConstStringHelper.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( Amplifier );
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( Amplifier, Mengine::AmplifierPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AmplifierPlugin::AmplifierPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AmplifierPlugin::~AmplifierPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AmplifierPlugin::_initializePlugin()
    {
        SERVICE_CREATE( Amplifier, MENGINE_DOCUMENT_FACTORABLE );

#ifdef MENGINE_USE_SCRIPT_SERVICE
        NOTIFICATION_ADDOBSERVERLAMBDA_THIS( NOTIFICATOR_SCRIPT_EMBEDDING, [this]()
        {
            SCRIPT_SERVICE()
                ->addScriptEmbedding( STRINGIZE_STRING_LOCAL( "AmplifierScriptEmbedding" ), Helper::makeFactorableUnique<AmplifierScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) );
        }, MENGINE_DOCUMENT_FACTORABLE );

        NOTIFICATION_ADDOBSERVERLAMBDA_THIS( NOTIFICATOR_SCRIPT_EJECTING, []()
        {
            SCRIPT_SERVICE()
                ->removeScriptEmbedding( STRINGIZE_STRING_LOCAL( "AmplifierScriptEmbedding" ) );
        }, MENGINE_DOCUMENT_FACTORABLE );
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AmplifierPlugin::_finalizePlugin()
    {
#ifdef MENGINE_USE_SCRIPT_SERVICE
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EMBEDDING );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EJECTING );
#endif

        SERVICE_FINALIZE( Amplifier );
    }
    //////////////////////////////////////////////////////////////////////////
    void AmplifierPlugin::_destroyPlugin()
    {
        SERVICE_DESTROY( Amplifier );
    }
    //////////////////////////////////////////////////////////////////////////
}