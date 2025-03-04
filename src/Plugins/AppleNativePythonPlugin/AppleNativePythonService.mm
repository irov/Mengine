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
    pybind::object AppleNativePythonService::addAppleCallback( const ConstString & _plugin, const ConstString & _method, const pybind::object & _cb, const pybind::args & _args )
    {
        MapApplePythonCallbacks::iterator it_found = m_callbacks.find( Utility::make_pair( _plugin, _method ) );

        if( it_found == m_callbacks.end() )
        {
            VectorApplePythonCallbacks new_callbacks;

            it_found = m_callbacks.emplace( Utility::make_pair( Utility::make_pair( _plugin, _method ), new_callbacks ) ).first;
        }

        VectorApplePythonCallbacks & callbacks = it_found->second;

        ApplePythonCallbackDesc desc;
        desc.cb = _cb;
        desc.args = _args;

        callbacks.emplace_back( desc );

        return _cb;
        
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleNativePythonService::removeAppleCallback( const ConstString & _plugin, const ConstString & _method, const pybind::object & _cb )
    {
        MapApplePythonCallbacks::iterator it_found = m_callbacks.find( Utility::make_pair( _plugin, _method ) );

        if( it_found == m_callbacks.end() )
        {
            LOGGER_ERROR("invalid remove apple callback plugin '%s' method '%s' not found"
                , _plugin.c_str()
                , _method.c_str()
            );

            return;
        }

        VectorApplePythonCallbacks & callbacks = it_found->second;

        VectorApplePythonCallbacks::iterator it_callback_found = Algorithm::find_if( callbacks.begin(), callbacks.end(), [_cb](const ApplePythonCallbackDesc & desc) {
            return desc.cb.ptr() == _cb.ptr();
        } );

        if( it_callback_found == callbacks.end() )
        {
            LOGGER_ERROR("invalid remove apple callback plugin '%s' method '%s' not found [cb]"
                , _plugin.c_str()
                , _method.c_str()
            );

            return;
        }

        callbacks.erase( it_callback_found );
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleNativePythonService::activateSemaphore( const ConstString & _name )
    {
        LOGGER_INFO( "apple", "activate semaphore '%s'"
            , _name.c_str()
        );

        NSString * ns_name = [AppleString NSStringFromConstString:_name];

        [AppleSemaphoreService.sharedInstance activateSemaphore:ns_name];
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
