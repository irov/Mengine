#include "cURLPlugin.h"

#include "Interface/FileServiceInterface.h"
#include "Interface/ScriptServiceInterface.h"
#include "Interface/NotificationServiceInterface.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/FactorableUnique.h"

#ifdef MENGINE_USE_SCRIPT_SERVICE
#include "cURLScriptEmbedding.h"
#endif

//////////////////////////////////////////////////////////////////////////
SERVICE_EXTERN( cURLService );
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( cURL, Mengine::cURLPlugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    cURLPlugin::cURLPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    cURLPlugin::~cURLPlugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool cURLPlugin::_initializePlugin()
    {
        SERVICE_CREATE( cURLService );

#ifdef MENGINE_USE_SCRIPT_SERVICE
        NOTIFICATION_ADDOBSERVERLAMBDA( NOTIFICATOR_SCRIPT_EMBEDDING, this, []()
        {
            ADD_SCRIPT_EMBEDDING( STRINGIZE_STRING_LOCAL( "cURLScriptEmbedding" ), Helper::makeFactorableUnique<cURLScriptEmbedding>() );
        } );
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLPlugin::_finalizePlugin()
    {
#ifdef MENGINE_USE_SCRIPT_SERVICE
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EMBEDDING );

        if( SERVICE_EXIST( ScriptServiceInterface ) == true )
        {
            REMOVE_SCRIPT_EMBEDDING( STRINGIZE_STRING_LOCAL( "cURLScriptEmbedding" ) );
        }
#endif

        SERVICE_FINALIZE( cURLService );
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLPlugin::_destroyPlugin()
    {
        SERVICE_DESTROY( cURLService );
    }
}