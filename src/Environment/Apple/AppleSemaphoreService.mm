#import "AppleSemaphoreService.h"

@implementation AppleSemaphoreService

+ (instancetype)sharedInstance {
    static AppleSemaphoreService * sharedInstance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedInstance = [[AppleSemaphoreService alloc] init];
    });
    return sharedInstance;
}

- (instancetype)init {
    if (self = [super init]) {
        self.m_semaphores = [[NSMutableDictionary alloc] init];
    }
    return self;
}

- (void)activateSemaphore:(NSString * _Nonnull)name withValue:(id _Nullable)value {
    NSMutableArray<AppleSemaphoreListenerNSProxy *> * listeners;
    @synchronized(self) {
        AppleSemaphore * semaphore = self.m_semaphores[name];

        if (semaphore == nil) {
            semaphore = [[AppleSemaphore alloc] initWithActivated:YES value:value];

            self.m_semaphores[name] = semaphore;

            return;
        }

        if ([semaphore isActivated] == YES) {
            return;
        }

        listeners = [semaphore activate:value];
    }

    for (AppleSemaphoreListenerNSProxy * proxy in listeners) {
        [proxy invoke:value];
    }
}

- (void)deactivateSemaphore:(NSString * _Nonnull)name {
    @synchronized(self) {
        [self.m_semaphores removeObjectForKey:name];
    }
}

- (void)waitSemaphore:(NSString * _Nonnull)name withListener:(AppleSemaphoreListenerNSProxy * _Nonnull)listener {
    id value;
    @synchronized(self) {
        AppleSemaphore * semaphore = self.m_semaphores[name];

        if (semaphore == nil) {
            semaphore = [[AppleSemaphore alloc] initWithActivated:NO value:nil];
            [semaphore addListener:listener];

            self.m_semaphores[name] = semaphore;

            return;
        }

        if ([semaphore isActivated] == NO) {
            [semaphore addListener:listener];

            return;
        }

        value = [semaphore getValue];
    }

    [listener invoke:value];
}

- (void)clearSemaphores {
    @synchronized(self) {
        [self.m_semaphores removeAllObjects];
    }
}

@end
