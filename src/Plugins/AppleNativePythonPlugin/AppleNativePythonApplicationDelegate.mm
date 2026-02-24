#import "AppleNativePythonApplicationDelegate.h"
#import "AppleNativePythonCallbackDesc.h"

#import "Environment/Apple/AppleSemaphoreListenerNSProxy.h"
#import "Environment/Apple/AppleSemaphoreService.h"

#include "Kernel/Logger.h"
#include "Kernel/ScriptEmbeddingHelper.h"

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
#   include "AppleNativePythonScriptEmbedding.h"
#endif

@implementation AppleNativePythonApplicationDelegate
{
    NSMutableDictionary<NSString *, NSMutableDictionary<NSString *, NSMutableArray<AppleNativePythonCallbackDesc *> *> *> * m_callbacks;
}

+ (instancetype)sharedInstance {
    static AppleNativePythonApplicationDelegate * sharedInstance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedInstance = [[AppleNativePythonApplicationDelegate alloc] init];
    });

    return sharedInstance;
}

- (instancetype)init {
    self = [super init];

    m_callbacks = [NSMutableDictionary dictionary];

    return self;
}

- (void)onFinalize {
    [m_callbacks removeAllObjects];
}

#if defined(MENGINE_BUILD_MENGINE_SCRIPT_EMBEDDED)
- (void)onRunBegin {
    Mengine::Helper::addScriptEmbedding<Mengine::AppleNativePythonScriptEmbedding>( MENGINE_DOCUMENT_FUNCTION );
}

- (void)onStopEnd {
    Mengine::Helper::removeScriptEmbedding<Mengine::AppleNativePythonScriptEmbedding>();

    [m_callbacks removeAllObjects];
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

    NSMutableDictionary<NSString *, NSMutableArray<AppleNativePythonCallbackDesc *> *> * pluginMethods = m_callbacks[plugin];
    if( pluginMethods == nil )
    {
        pluginMethods = [NSMutableDictionary dictionary];
        m_callbacks[plugin] = pluginMethods;
    }

    NSMutableArray<AppleNativePythonCallbackDesc *> * callbacks = pluginMethods[method];
    if( callbacks == nil )
    {
        callbacks = [NSMutableArray array];
        pluginMethods[method] = callbacks;
    }

    AppleNativePythonCallbackDesc * desc = [[AppleNativePythonCallbackDesc alloc] initWithCallback:cb args:args];
    [callbacks addObject:desc];

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

    NSMutableDictionary<NSString *, NSMutableArray<AppleNativePythonCallbackDesc *> *> * pluginMethods = m_callbacks[plugin];
    if( pluginMethods == nil )
    {
        LOGGER_ERROR( "invalid remove apple callback plugin '%s' method '%s' not found"
            , [plugin UTF8String]
            , [method UTF8String]
        );

        return;
    }

    NSMutableArray<AppleNativePythonCallbackDesc *> * callbacks = pluginMethods[method];
    if( callbacks == nil )
    {
        LOGGER_ERROR( "invalid remove apple callback plugin '%s' method '%s' not found"
            , [plugin UTF8String]
            , [method UTF8String]
        );

        return;
    }

    NSUInteger index = NSNotFound;
    for( NSUInteger i = 0; i != callbacks.count; ++i )
    {
        AppleNativePythonCallbackDesc * desc = callbacks[i];

        if( desc->m_cb.ptr() == cb.ptr() )
        {
            index = i;
            break;
        }
    }

    if( index == NSNotFound )
    {
        LOGGER_ERROR( "invalid remove apple callback plugin '%s' method '%s' not found [cb]"
            , [plugin UTF8String]
            , [method UTF8String]
        );

        return;
    }

    [callbacks removeObjectAtIndex:index];

    if( callbacks.count == 0 )
    {
        [pluginMethods removeObjectForKey:method];
    }

    if( pluginMethods.count == 0 )
    {
        [m_callbacks removeObjectForKey:plugin];
    }
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
