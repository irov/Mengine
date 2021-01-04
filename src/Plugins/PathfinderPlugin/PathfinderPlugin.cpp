#include "PathfinderPlugin.h"

#include "Interface/PrototypeServiceInterface.h"
#include "Interface/ScriptServiceInterface.h"
#include "Interface/NotificationServiceInterface.h"

#ifdef MENGINE_USE_SCRIPT_SERVICE
#   include "PathfinderScriptEmbedding.h"
#endif

#include "Kernel/ModuleFactory.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/NodePrototypeGenerator.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( PathfinderService );
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( Pathfinder, Mengine::PathfinderPlugin )
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    PathfinderPlugin::PathfinderPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PathfinderPlugin::~PathfinderPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool PathfinderPlugin::_initializePlugin()
    {
        SERVICE_CREATE( PathfinderService, MENGINE_DOCUMENT_FACTORABLE );

#ifdef MENGINE_USE_SCRIPT_SERVICE
        NOTIFICATION_ADDOBSERVERLAMBDA_THIS( NOTIFICATOR_SCRIPT_EMBEDDING, [this]()
        {
            ADD_SCRIPT_EMBEDDING( STRINGIZE_STRING_LOCAL( "PathfinderScriptEmbedding" ), Helper::makeFactorableUnique<PathfinderScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) );
        }, MENGINE_DOCUMENT_FACTORABLE );

        NOTIFICATION_ADDOBSERVERLAMBDA_THIS( NOTIFICATOR_SCRIPT_EJECTING, []()
        {
            REMOVE_SCRIPT_EMBEDDING( STRINGIZE_STRING_LOCAL( "PathfinderScriptEmbedding" ) );
        }, MENGINE_DOCUMENT_FACTORABLE );
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PathfinderPlugin::_finalizePlugin()
    {
#ifdef MENGINE_USE_SCRIPT_SERVICE
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EMBEDDING );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EJECTING );
#endif
    }
    //////////////////////////////////////////////////////////////////////////
}
