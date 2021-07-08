#include "AreaOfInterestPlugin.h"

#include "Interface/PrototypeServiceInterface.h"
#include "Interface/ScriptServiceInterface.h"
#include "Interface/NotificationServiceInterface.h"

#include "NodeAreaOfInterestTrigger.h"
#include "NodeAreaOfInterestActor.h"

#ifdef MENGINE_USE_SCRIPT_SERVICE
#   include "AreaOfInterestScriptEmbedding.h"
#endif

#include "Kernel/ConstStringHelper.h"
#include "Kernel/NodePrototypeGenerator.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( AreaOfInterestService );
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( AreaOfInterest, Mengine::AreaOfInterestPlugin )
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AreaOfInterestPlugin::AreaOfInterestPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AreaOfInterestPlugin::~AreaOfInterestPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AreaOfInterestPlugin::_initializePlugin()
    {
        SERVICE_CREATE( AreaOfInterestService, MENGINE_DOCUMENT_FACTORABLE );

#ifdef MENGINE_USE_SCRIPT_SERVICE
        NOTIFICATION_ADDOBSERVERLAMBDA_THIS( NOTIFICATOR_SCRIPT_EMBEDDING, [this]()
        {
            SCRIPT_SERVICE()
                ->addScriptEmbedding( STRINGIZE_STRING_LOCAL( "AreaOfInterestScriptEmbedding" ), Helper::makeFactorableUnique<AreaOfInterestScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) );
        }, MENGINE_DOCUMENT_FACTORABLE );

        NOTIFICATION_ADDOBSERVERLAMBDA_THIS( NOTIFICATOR_SCRIPT_EJECTING, []()
        {
            SCRIPT_SERVICE()
                ->removeScriptEmbedding( STRINGIZE_STRING_LOCAL( "Movie2ScriptEmbedding" ) );
        }, MENGINE_DOCUMENT_FACTORABLE );
#endif

        PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "NodeAreaOfInterestTrigger" ), Helper::makeFactorableUnique<NodePrototypeGenerator<NodeAreaOfInterestTrigger, 32>>( MENGINE_DOCUMENT_FACTORABLE ) );

        PROTOTYPE_SERVICE()
            ->addPrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "NodeAreaOfInterestActor" ), Helper::makeFactorableUnique<NodePrototypeGenerator<NodeAreaOfInterestActor, 32>>( MENGINE_DOCUMENT_FACTORABLE ) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AreaOfInterestPlugin::_finalizePlugin()
    {
#ifdef MENGINE_USE_SCRIPT_SERVICE
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EMBEDDING );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EJECTING );
#endif

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "NodeAreaOfInterestTrigger" ) );

        PROTOTYPE_SERVICE()
            ->removePrototype( STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "NodeAreaOfInterestTrigger" ) );


        SERVICE_FINALIZE( AreaOfInterestService );
    }
    //////////////////////////////////////////////////////////////////////////
}
