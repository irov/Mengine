#import "AppleFirebaseCrashlyticsANRMonitor.h"

#import <FirebaseCrashlytics/FirebaseCrashlytics.h>

#import <mach/mach.h>

@implementation AppleFirebaseCrashlyticsANRMonitor

static void runLoopObserverCallBack(CFRunLoopObserverRef observer, CFRunLoopActivity activity, void *info) {
    static CFAbsoluteTime lastTime = 0;

    if (activity == kCFRunLoopBeforeSources || activity == kCFRunLoopBeforeWaiting) {
        lastTime = CFAbsoluteTimeGetCurrent();
    }

    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_BACKGROUND, 0), ^{
        sleep(10);
        
        CFAbsoluteTime elapsed = CFAbsoluteTimeGetCurrent() - lastTime;
        if (elapsed >= 10.0) {
            NSArray * stackTraceArray = [NSThread callStackSymbols];
            NSString * stackTrace = [stackTraceArray componentsJoinedByString:@"\n"];
            NSString * message = [NSString stringWithFormat:@"Application Not Responding %.2f seconfs (ANR)", elapsed];
            
            NSLog(@"%@\n%@", message, stackTrace);

            NSDictionary * userInfo = @{
                NSLocalizedDescriptionKey:message,
                NSLocalizedFailureReasonErrorKey:stackTrace
            };

            NSError * error = [NSError errorWithDomain:@"com.mengine.firebase"
                                                  code:1001
                                              userInfo:userInfo];

            [[FIRCrashlytics crashlytics] recordError:error];
        }
    });
}

- (instancetype)init {
    self = [super init];
    if (self) {
        CFRunLoopObserverContext context = {0, (__bridge void *)self, NULL, NULL, NULL};

        self.m_observer = (__bridge_transfer id)CFRunLoopObserverCreate(kCFAllocatorDefault
            , kCFRunLoopBeforeSources | kCFRunLoopBeforeWaiting
            , true
            , 0
            , &runLoopObserverCallBack
            , &context
        );

        if (self.m_observer != nil) {
            CFRunLoopAddObserver(CFRunLoopGetMain(), (__bridge CFRunLoopObserverRef)self.m_observer, kCFRunLoopCommonModes);
        }
    }
    return self;
}

- (void)dealloc  {
    if (self.m_observer != nil) {
        CFRunLoopRemoveObserver(CFRunLoopGetMain(), (__bridge CFRunLoopObserverRef)self.m_observer, kCFRunLoopCommonModes);
        self.m_observer = nil;
    }
}

@end
