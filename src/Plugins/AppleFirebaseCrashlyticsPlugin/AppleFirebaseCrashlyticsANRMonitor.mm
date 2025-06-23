#import "AppleFirebaseCrashlyticsANRMonitor.h"

#import "Environment/iOS/iOSLog.h"

#import <FirebaseCrashlytics/FirebaseCrashlytics.h>

@implementation AppleFirebaseCrashlyticsANRMonitor

- (void)runANRMainWatcher {
    dispatch_async(dispatch_get_main_queue(), ^{
        thread_t thread = pthread_mach_thread_np(pthread_self());
        
        [self runANRWatcher:thread];
    });
}

static void RunLoopObserverCallback(CFRunLoopObserverRef observer, CFRunLoopActivity activity, void *info) {
    dispatch_semaphore_t semaphore = (__bridge dispatch_semaphore_t)info;
    dispatch_semaphore_signal(semaphore);
}

- (void)runANRWatcher:(thread_act_t)UIThread {
    dispatch_semaphore_t semaphore = dispatch_semaphore_create(0);
    
    [[NSNotificationCenter defaultCenter] addObserverForName:UIApplicationWillResignActiveNotification
                    object:nil
                     queue:nil
                usingBlock:^(__unused NSNotification *note) {
        m_monitoringEnabled = NO;
        dispatch_semaphore_signal(semaphore);
    }];

    [[NSNotificationCenter defaultCenter] addObserverForName:UIApplicationDidBecomeActiveNotification
                    object:nil
                     queue:nil
                usingBlock:^(__unused NSNotification *note) {
        monitoringEnabled = YES;
        dispatch_semaphore_signal(semaphore);
    }];
    
    CFRunLoopObserverContext context = {0, (__bridge void *)semaphore, NULL, NULL, NULL};
    CFRunLoopObserverRef observer = CFRunLoopObserverCreate(kCFAllocatorDefault, kCFRunLoopBeforeSources | kCFRunLoopAfterWaiting, true, 0, &RunLoopObserverCallback, &sContext);
    CFRunLoopAddObserver(CFRunLoopGetMain(), observer, kCFRunLoopCommonModes);
    CFRelease(observer);
    
    dispatch_async(dispatch_get_global_queue(QOS_CLASS_USER_INITIATED, 0), ^{
        while (true) {
            long result = dispatch_semaphore_wait(sSemaphore, dispatch_time(DISPATCH_TIME_NOW, (int64_t)(10 * NSEC_PER_SEC)));
            
            if (result == 0) {
                continue;
            }
            
            if (monitoringEnabled == NO) {
                dispatch_semaphore_signal(semaphore);
                continue;
            }
            
            [AppleFirebaseCrashlyticsANRMonitor backtraceOfThread:thread];
        }
    });
}

static size_t my_backtrace_from_fp( _STRUCT_ARM_THREAD_STATE64 _state, void ** _buffer, size_t _size ) {
    void **fp = (void **)(uintptr_t)_state.__fp;
    void *pc = (void *)(uintptr_t)_state.__pc;

    if (fp == NULL || pc == NULL) {
        return 0;
    }

    size_t i = 0;

    _buffer[i++] = pc;

    while (fp && i < _size) {
        void *saved_pc = *(fp + 1);

        if (saved_pc == NULL) break;

        _buffer[i++] = saved_pc;

        void **next_fp = (void **)(*fp);

        if (next_fp <= fp || ((uintptr_t)next_fp & 0xF)) {
            break;
        }

        fp = next_fp;
    }

    return i;
}

+ (BOOL)backtraceOfMainThread:(thread_t)thread {
#if defined(__aarch64__)
    _STRUCT_ARM_THREAD_STATE64 state = {0};
    mach_msg_type_number_t count = ARM_THREAD_STATE64_COUNT;
    kern_return_t kr = thread_get_state(thread, ARM_THREAD_STATE64, (thread_state_t)&state, &count);
    if (kr != KERN_SUCCESS) {
        return NO;
    }
    
    void * buffer[128] = {NULL};
    
    size_t countFrames = my_backtrace_from_fp(state, buffer, 128);

    if (countFrames == 0) {
        return NO;
    }
    
#ifdef DEBUG
    char **symbols = backtrace_symbols(buffer, countFrames);
    NSMutableArray<NSString *> *callStackSymbols = [NSMutableArray arrayWithCapacity:countFrames];
    for (int i = 0; i < countFrames; ++i) {
        if (symbols[i]) {
            [callStackSymbols addObject:[NSString stringWithUTF8String:symbols[i]]];
        } else {
            [callStackSymbols addObject:@"<null>"];
        }
    }
    free(symbols);
    IOS_LOGGER_MESSAGE(@"[ANR] detected on main thread: %@", callStackSymbols);
#endif
    
    FIRExceptionModel *exceptionModel = [[FIRExceptionModel alloc] initWithName:@"ANRException"
                                                                         reason:@"ANR detected on main thread"];
    
    NSMutableArray<FIRStackFrame *> *frames = [NSMutableArray arrayWithCapacity:countFrames];
    for (int i = 0; i < countFrames; ++i) {
        uintptr_t address = (uintptr_t)buffer[i];
        FIRStackFrame *frame = [FIRStackFrame stackFrameWithAddress:address];
        [frames addObject:frame];
    }
    
    exceptionModel.stackTrace = frames;

    [[FIRCrashlytics crashlytics] recordExceptionModel:exceptionModel];
    return YES;
#else
    return NO;
#endif
}