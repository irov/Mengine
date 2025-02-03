#import <FirebaseCrashlytics/FirebaseCrashlytics.h>
#import <mach/mach.h>

#import "AppleFirebaseCrashlyticsANRMonitor.h"

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
            NSString * message = [NSString stringWithFormat:@"Application Not Responding %.2f seconfs (ANR)", elapsed]
            
            NSLog(@"%@\n%@", message, stackTrace);

            NSDictionary * userInfo = @{
                NSLocalizedDescriptionKey:message
            };

            NSError * error = [NSError errorWithDomain:@"com.mengine.firebase"
                                                 code:1001
                                             userInfo:userInfo,
                     NSLocalizedFailureReasonErrorKey:stackTrace}];

            [[FIRCrashlytics crashlytics] recordError:error];
        }
    });
}

- (instancetype)init {
    self = [super init];
    if (self) {
        CFRunLoopObserverContext context = {0, (__bridge void *)self, NULL, NULL, NULL};

        m_observer = CFRunLoopObserverCreate(kCFAllocatorDefault
            , kCFRunLoopBeforeSources | kCFRunLoopBeforeWaiting
            , true
            , 0
            , &runLoopObserverCallBack
            , &context
        );

        if (m_observer) {
            CFRunLoopAddObserver(CFRunLoopGetMain(), m_observer, kCFRunLoopCommonModes);
        }
    }
    return self;
}

- (void)dealloc  {
    if (m_observer != nil) {
        CFRunLoopRemoveObserver(CFRunLoopGetMain(), m_observer, kCFRunLoopCommonModes);
        CFRelease(m_observer);
        m_observer = nil;
    }
}

@end