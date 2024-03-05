#import "AppleSemaphoreService.h"

@implementation AppleSemaphoreService

+ (instancetype)sharedInstance {
    static AppleSemaphoreService *sharedInstance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedInstance = [[self alloc] init];
    });
    return sharedInstance;
}

- (instancetype)init {
    if (self = [super init]) {
        self.m_semaphores = [[NSMutableDictionary alloc] init];
    }
    return self;
}

- (void)activateSemaphore:(NSString *)name {
    AppleSemaphore *semaphore = self.m_semaphores[name];
    
    NSMutableArray<AppleSemaphoreListenerNSProxy *> * listeners = nil;
    
    @synchronized(self) {
        if (semaphore == nil) {
            semaphore = [[AppleSemaphore alloc] initWithActivated:NO];
            
            self.m_semaphores[name] = semaphore;
            
            return;
        }
        
        if ([semaphore isActivated] == YES) {
            return;
        }
        
        listeners = [semaphore activate];
    }
    
    for (AppleSemaphoreListenerNSProxy * proxy in listeners) {
        [proxy invoke];
    }
}

- (void)deactivateSemaphore:(NSString *)name {
    @synchronized(self) {
        [self.m_semaphores removeObjectForKey:name];
    }
}

- (void)waitSemaphore:(NSString *)name withListener:(AppleSemaphoreListenerNSProxy * _Nonnull)listener {
    @synchronized(self) {
        AppleSemaphore * semaphore = self.m_semaphores[name];
        
        if (semaphore == nil) {
            semaphore = [[AppleSemaphore alloc] initWithActivated:NO];
            [semaphore addListener:listener];
            
            self.m_semaphores[name] = semaphore;
            return;
        }
        
        if ([semaphore isActivated] == NO) {
            [semaphore addListener:listener];
            return;
        }
    }
    
    [listener invoke];
}

@end
