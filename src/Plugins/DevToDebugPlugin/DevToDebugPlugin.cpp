#include "DevToDebugPlugin.h"

#include "Interface/ScriptServiceInterface.h"

#include "Kernel/ConfigHelper.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/NotificationHelper.h"

#ifdef MENGINE_USE_SCRIPT_SERVICE
#include "DevToDebugScriptEmbedding.h"
#endif


//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( DevToDebugService );
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( DevToDebug, Mengine::DevToDebugPlugin )
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    DevToDebugPlugin::DevToDebugPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    DevToDebugPlugin::~DevToDebugPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool DevToDebugPlugin::_unimportantPlugin() const
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DevToDebugPlugin::_availablePlugin() const
    {
        if( HAS_OPTION( "nodevtodebug" ) == true )
        {
            return false;
        }

        if( HAS_OPTION( "devtodebug" ) == false )
        {
            if( CONFIG_VALUE( "DevToDebug", "Enable", true ) == false )
            {
                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool DevToDebugPlugin::_initializePlugin()
    {
        if( SERVICE_CREATE( DevToDebugService, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

#ifdef MENGINE_USE_SCRIPT_SERVICE
        NOTIFICATION_ADDOBSERVERLAMBDA_THIS( NOTIFICATOR_SCRIPT_EMBEDDING, [this]()
        {
            SCRIPT_SERVICE()
                ->addScriptEmbedding( STRINGIZE_STRING_LOCAL( "DevToDebugScriptEmbedding" ), Helper::makeFactorableUnique<DevToDebugScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) );
        }, MENGINE_DOCUMENT_FACTORABLE );

        NOTIFICATION_ADDOBSERVERLAMBDA_THIS( NOTIFICATOR_SCRIPT_EJECTING, []()
        {
            SCRIPT_SERVICE()
                ->removeScriptEmbedding( STRINGIZE_STRING_LOCAL( "DevToDebugScriptEmbedding" ) );
        }, MENGINE_DOCUMENT_FACTORABLE );
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugPlugin::_finalizePlugin()
    {
#ifdef MENGINE_USE_SCRIPT_SERVICE
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EMBEDDING );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EJECTING );
#endif

        SERVICE_FINALIZE( DevToDebugService );
    }
    //////////////////////////////////////////////////////////////////////////
    void DevToDebugPlugin::_destroyPlugin()
    {
        SERVICE_DESTROY( DevToDebugService );
    }
    //////////////////////////////////////////////////////////////////////////
}
