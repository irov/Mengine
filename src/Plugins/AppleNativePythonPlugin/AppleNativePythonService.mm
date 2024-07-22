#include "AppleNativePythonService.h"

#include "Interface/ScriptServiceInterface.h"

#import "Environment/Apple/AppleSemaphoreListenerNSProxy.h"
#import "Environment/Apple/AppleSemaphoreService.h"
#import "Environment/Apple/AppleString.h"

#include "AppleNativePythonScriptEmbedding.h"

#include "Kernel/NotificationHelper.h"
#include "Kernel/Logger.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( AppleNativePythonService, Mengine::AppleNativePythonService );
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleNativePythonService::AppleNativePythonService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleNativePythonService::~AppleNativePythonService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AppleNativePythonService::_initializeService()
    {
        NOTIFICATION_ADDOBSERVERLAMBDA_THIS( NOTIFICATOR_SCRIPT_EMBEDDING, [this]()
        {
            SCRIPT_SERVICE()
                ->addScriptEmbedding( STRINGIZE_STRING_LOCAL( "AppleNativePythonScriptEmbedding" ), Helper::makeFactorableUnique<AppleNativePythonScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE ) );
        }, MENGINE_DOCUMENT_FACTORABLE );

        NOTIFICATION_ADDOBSERVERLAMBDA_THIS( NOTIFICATOR_SCRIPT_EJECTING, []()
        {
            SCRIPT_SERVICE()
                ->removeScriptEmbedding( STRINGIZE_STRING_LOCAL( "AppleNativePythonScriptEmbedding" ) );
        }, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleNativePythonService::_finalizeService()
    {
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EMBEDDING );
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_SCRIPT_EJECTING );
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleNativePythonService::waitSemaphore( const ConstString & _name, const AppleSemaphoreListenerInterfacePtr & _listener )
    {
        LOGGER_INFO( "apple", "wait semaphore '%s'"
            , _name.c_str()
        );
        
        NSString * ns_name = [AppleString NSStringFromConstString:_name];

        AppleSemaphoreListenerNSProxy * proxy = [[AppleSemaphoreListenerNSProxy alloc] initWithListener:_listener];
        
        [AppleSemaphoreService.sharedInstance waitSemaphore:ns_name withListener:proxy];
    }
    //////////////////////////////////////////////////////////////////////////
}
