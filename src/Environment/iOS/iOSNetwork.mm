#import "iOSNetwork.h"

#import "Environment/Apple/AppleLog.h"

@implementation iOSNetwork

+ (instancetype)sharedInstance {
    static iOSNetwork *sharedInstance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedInstance = [[iOSNetwork alloc] init];
    });
    return sharedInstance;
}

- (instancetype)init {
    self = [super init];
    if (self) {
        self.m_available = YES;
        self.m_monitor = nw_path_monitor_create();
        self.m_queue = dispatch_queue_create("MengineNetworkMonitorQueue", DISPATCH_QUEUE_SERIAL);
    }
    return self;
}

- (void)startMonitoring {
    nw_path_monitor_set_update_handler(self.m_monitor, ^(nw_path_t path) {
        if (nw_path_get_status(path) == nw_path_status_satisfied) {
            self.m_available = YES;
        } else {
            self.m_available = NO;
        }
        
        [AppleLog withFormat:@"Network available: %@", (self.m_available == YES) ? @"YES" : @"NO"];
    });

    nw_path_monitor_set_queue(self.m_monitor, self.m_queue);
    
    nw_path_monitor_start(self.m_monitor);
}

- (void)stopMonitoring {
    nw_path_monitor_cancel(self.m_monitor);
}

- (BOOL)isNetworkAvailable {
    return self.m_available;
}

@end
