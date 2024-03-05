#import "AppleSemaphore.h"

@implementation AppleSemaphoreListenerNSProxy

- (instancetype _Nonnull) initWithListener:(const Mengine::AppleSemaphoreListenerInterfacePtr &)_proxy {
    self = [super init];
    if (self) {
        self.m_proxy = _proxy;
    }
    return self;
}

- (void) invoke {
    self.m_proxy->invoke();
}

@end
