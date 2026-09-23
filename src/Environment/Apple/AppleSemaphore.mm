#import "AppleSemaphore.h"

@implementation AppleSemaphore

- (instancetype)initWithActivated:(BOOL)activated value:(id)value {
    self = [super init];
    if (self) {
        self.m_activated = activated;
        self.m_value = value;
        self.m_listeners = [NSMutableArray array];
    }
    return self;
}

- (BOOL)isActivated {
    return self.m_activated;
}

- (id)getValue {
    return self.m_value;
}

- (NSMutableArray<AppleSemaphoreListenerNSProxy *> *)activate:(id)value {
    self.m_value = value;
    self.m_activated = YES;
    
    NSMutableArray<AppleSemaphoreListenerNSProxy*> * listeners = self.m_listeners;
    
    self.m_listeners = nil;
    
    return listeners;
}

- (void)addListener:(AppleSemaphoreListenerNSProxy *)listener {
    [self.m_listeners addObject:listener];
}

@end
