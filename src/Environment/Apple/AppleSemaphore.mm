#import "AppleSemaphore.h"

@implementation AppleSemaphore

- (instancetype)initWithActivated:(BOOL)activated {
    self = [super init];
    if (self) {
        self.m_activated = activated;
        self.m_listeners = [NSMutableArray array];
    }
    return self;
}

- (BOOL)isActivated {
    return self.m_activated;
}

- (NSMutableArray<AppleSemaphoreListenerNSProxy *> *)activate {
    self.m_activated = YES;
    
    NSMutableArray<AppleSemaphoreListenerNSProxy*> * listeners = self.m_listeners;
    
    self.m_listeners = nil;
    
    return listeners;
}

- (void)addListener:(AppleSemaphoreListenerNSProxy *)listener {
    [self.m_listeners addObject:listener];
}

@end
