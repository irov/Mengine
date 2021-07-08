#include "cURLPlugin.h"

#include "Interface/FileServiceInterface.h"
#include "Interface/ScriptServiceInterface.h"
#include "Interface/NotificationServiceInterface.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/AssertionAllocator.h"

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
        SERVICE_CREATE( cURLService, MENGINE_DOCUMENT_FACTORABLE );

#ifdef MENGINE_USE_SCRIPT_SERVICE
        NOTIFICATION_ADDOBSERVERLAMBDA_THIS( NOTIFICATOR_SCRIPT_EMBEDDING, [this]()
        {
            SCRIPT_SERVICE()
                ->addScriptEmbedding( STRINGIZE_STRING_LOCAL( "cURLScriptEmbedding" ), Helper::makeFactorableUnique<cURLScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) );
        }, MENGINE_DOCUMENT_FACTORABLE );

        NOTIFICATION_ADDOBSERVERLAMBDA_THIS( NOTIFICATOR_SCRIPT_EJECTING, []()
        {
            SCRIPT_SERVICE()
                ->removeScriptEmbedding( STRINGIZE_STRING_LOCAL( "cURLScriptEmbedding" ) );
        }, MENGINE_DOCUMENT_FACTORABLE );
#endif

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLPlugin::_finalizePlugin()
    {
#ifdef MENGINE_USE_SCRIPT_SERVICE
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EMBEDDING );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EJECTING );
#endif

        SERVICE_FINALIZE( cURLService );
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLPlugin::_destroyPlugin()
    {
        SERVICE_DESTROY( cURLService );

        MENGINE_ASSERTION_ALLOCATOR( "curl" );
    }
    //////////////////////////////////////////////////////////////////////////
}