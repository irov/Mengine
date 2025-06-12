#include "AndroidNativePythonPlugin.h"

#include "Interface/ScriptServiceInterface.h"

#include "AndroidNativePythonService.h"
#include "AndroidNativePythonScriptEmbedding.h"

#include "Kernel/ModuleFactory.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/PluginHelper.h"
#include "Kernel/NotificationHelper.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( AndroidNativePythonService );
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( AndroidNativePython, Mengine::AndroidNativePythonPlugin );
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AndroidNativePythonPlugin::AndroidNativePythonPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AndroidNativePythonPlugin::~AndroidNativePythonPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AndroidNativePythonPlugin::_initializePlugin()
    {
        if( SERVICE_CREATE( AndroidNativePythonService, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            return false;
        }

#ifdef MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED
        NOTIFICATION_ADDOBSERVERLAMBDA_THIS( NOTIFICATOR_SCRIPT_EMBEDDING, [this]()
        {
            SCRIPT_SERVICE()
                ->addScriptEmbedding( STRINGIZE_STRING_LOCAL( "AndroidNativePythonScriptEmbedding" ), Helper::makeFactorableUnique<AndroidNativePythonScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) );
        }, MENGINE_DOCUMENT_FACTORABLE );

        NOTIFICATION_ADDOBSERVERLAMBDA_THIS( NOTIFICATOR_SCRIPT_EJECTING, []()
        {
            SCRIPT_SERVICE()
                ->removeScriptEmbedding( STRINGIZE_STRING_LOCAL( "AndroidNativePythonScriptEmbedding" ) );
        }, MENGINE_DOCUMENT_FACTORABLE );
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativePythonPlugin::_finalizePlugin()
    {
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EMBEDDING );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EJECTING );

        SERVICE_FINALIZE( AndroidNativePythonService );
    }
    //////////////////////////////////////////////////////////////////////////
    void AndroidNativePythonPlugin::_destroyPlugin()
    {
        SERVICE_DESTROY( AndroidNativePythonService );
    }
    ///////////////////////////////////////////////////////////////////////
}
