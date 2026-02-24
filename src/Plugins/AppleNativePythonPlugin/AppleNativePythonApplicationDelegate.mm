#import "AppleNativePythonApplicationDelegate.h"

#import "Environment/Apple/AppleSemaphoreListenerNSProxy.h"
#import "Environment/Apple/AppleSemaphoreService.h"
#import "Environment/Apple/AppleString.h"

#include "Kernel/Logger.h"
#include "Kernel/ConstString.h"
#include "Kernel/Map.h"
#include "Kernel/Vector.h"

#include "Config/StdAlgorithm.h"

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
#   include "AppleNativePythonScriptEmbedding.h"
#endif

namespace Mengine
{
    struct AppleNativePythonCallbackDesc
    {
        pybind::object cb;
        pybind::args args;
    };

    typedef Vector<AppleNativePythonCallbackDesc> VectorAppleNativePythonCallbacks;
    typedef Map<Pair<ConstString, ConstString>, VectorAppleNativePythonCallbacks> MapAppleNativePythonCallbacks;
}

@implementation AppleNativePythonApplicationDelegate
{
    Mengine::MapAppleNativePythonCallbacks m_callbacks;
}

+ (instancetype)sharedInstance {
    static AppleNativePythonApplicationDelegate * sharedInstance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedInstance = [[AppleNativePythonApplicationDelegate alloc] init];
    });

    return sharedInstance;
}

- (void)onFinalize {
    m_callbacks.clear();
}

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
- (void)onRunBegin {
    Mengine::Helper::addScriptEmbedding<Mengine::AppleNativePythonScriptEmbedding>( MENGINE_DOCUMENT_FACTORABLE );
}

- (void)onStopEnd {
    Mengine::Helper::removeScriptEmbedding<Mengine::AppleNativePythonScriptEmbedding>();

    m_callbacks.clear();
}
#endif

- (pybind::object)addAppleCallback:(NSString *)plugin
                             method:(NSString *)method
                           callback:(const pybind::object &)cb
                               args:(const pybind::args &)args {
    if( plugin == nil )
    {
        plugin = @"";
    }

    if( method == nil )
    {
        method = @"";
    }

    Mengine::ConstString plugin_cs = [AppleString NSStringToConstString:plugin];
    Mengine::ConstString method_cs = [AppleString NSStringToConstString:method];

    Mengine::MapAppleNativePythonCallbacks::iterator it_found = m_callbacks.find( Mengine::StdUtility::make_pair( plugin_cs, method_cs ) );

    if( it_found == m_callbacks.end() )
    {
        Mengine::VectorAppleNativePythonCallbacks new_callbacks;

        it_found = m_callbacks.emplace( Mengine::StdUtility::make_pair( Mengine::StdUtility::make_pair( plugin_cs, method_cs ), new_callbacks ) ).first;
    }

    Mengine::VectorAppleNativePythonCallbacks & callbacks = it_found->second;

    Mengine::AppleNativePythonCallbackDesc desc;
    desc.cb = cb;
    desc.args = args;

    callbacks.emplace_back( desc );

    return cb;
}

- (void)removeAppleCallback:(NSString *)plugin
                     method:(NSString *)method
                   callback:(const pybind::object &)cb {
    if( plugin == nil )
    {
        plugin = @"";
    }

    if( method == nil )
    {
        method = @"";
    }

    Mengine::ConstString plugin_cs = [AppleString NSStringToConstString:plugin];
    Mengine::ConstString method_cs = [AppleString NSStringToConstString:method];

    Mengine::MapAppleNativePythonCallbacks::iterator it_found = m_callbacks.find( Mengine::StdUtility::make_pair( plugin_cs, method_cs ) );

    if( it_found == m_callbacks.end() )
    {
        LOGGER_ERROR( "invalid remove apple callback plugin '%s' method '%s' not found"
            , plugin_cs.c_str()
            , method_cs.c_str()
        );

        return;
    }

    Mengine::VectorAppleNativePythonCallbacks & callbacks = it_found->second;

    Mengine::VectorAppleNativePythonCallbacks::iterator it_callback_found = Mengine::StdAlgorithm::find_if( callbacks.begin(), callbacks.end(), [cb](const Mengine::AppleNativePythonCallbackDesc & desc) {
        return desc.cb.ptr() == cb.ptr();
    } );

    if( it_callback_found == callbacks.end() )
    {
        LOGGER_ERROR( "invalid remove apple callback plugin '%s' method '%s' not found [cb]"
            , plugin_cs.c_str()
            , method_cs.c_str()
        );

        return;
    }

    callbacks.erase( it_callback_found );
}

- (void)activateSemaphore:(NSString *)name {
    if( name == nil )
    {
        name = @"";
    }

    LOGGER_INFO( "apple", "activate semaphore '%s'"
        , [name UTF8String]
    );

    [AppleSemaphoreService.sharedInstance activateSemaphore:name];
}

- (Mengine::AppleSemaphoreListenerInterfacePtr)waitSemaphore:(NSString *)name
                                                    listener:(const Mengine::AppleSemaphoreListenerInterfacePtr &)listener {
    if( name == nil )
    {
        name = @"";
    }

    LOGGER_INFO( "apple", "wait semaphore '%s'"
        , [name UTF8String]
    );

    AppleSemaphoreListenerNSProxy * proxy = [[AppleSemaphoreListenerNSProxy alloc] initWithListener:listener];

    [AppleSemaphoreService.sharedInstance waitSemaphore:name withListener:proxy];

    return listener;
}

@end
