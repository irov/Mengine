#import "AppleFirebaseCrashlyticsANRMonitor.h"

#import "Environment/iOS/iOSLog.h"
#import "Environment/Apple/AppleDetail.h"

#include "Config/StdString.h"

#import <UIKit/UIKit.h>
#import <FirebaseCrashlytics/FirebaseCrashlytics.h>

#import <mach/mach.h>
#import <pthread.h>

#if defined(MENGINE_DEBUG)
#   import <execinfo.h>
#endif

@implementation AppleFirebaseCrashlyticsANRMonitor

+ (instancetype)sharedInstance {
    static AppleFirebaseCrashlyticsANRMonitor *sharedInstance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedInstance = [[AppleFirebaseCrashlyticsANRMonitor alloc] init];
    });
    return sharedInstance;
}

- (instancetype)init {
    self = [super init];
    if (self) {
        self.m_monitoringEnabled = YES;
        [self runANRMainWatcher];
    }
    return self;
}

- (void)runANRMainWatcher {
    [AppleDetail addMainQueueOperation:^{
        pthread_t thread = pthread_self();
        
        [self runANRWatcher:thread withName:@"Main"];
    }];
}

static void RunLoopObserverCallback(CFRunLoopObserverRef observer, CFRunLoopActivity activity, void *info) {
    dispatch_semaphore_t semaphore = (__bridge dispatch_semaphore_t)info;
    dispatch_semaphore_signal(semaphore);
}

- (void)runANRWatcher:(pthread_t)thread withName:(NSString *)name {
    dispatch_semaphore_t semaphore = dispatch_semaphore_create(0);
    
    [[NSNotificationCenter defaultCenter] addObserverForName:UIApplicationWillResignActiveNotification
                                                      object:nil
                                                       queue:nil
                                                  usingBlock:^(__unused NSNotification *note) {
        self.m_monitoringEnabled = NO;
        dispatch_semaphore_signal(semaphore);
    }];
    
    [[NSNotificationCenter defaultCenter] addObserverForName:UIApplicationDidBecomeActiveNotification
                                                      object:nil
                                                       queue:nil
                                                  usingBlock:^(__unused NSNotification *note) {
        self.m_monitoringEnabled = YES;
        dispatch_semaphore_signal(semaphore);
    }];
    
    CFRunLoopObserverContext context = {0, (__bridge void *)semaphore, NULL, NULL, NULL};
    CFRunLoopObserverRef observer = CFRunLoopObserverCreate(kCFAllocatorDefault, kCFRunLoopBeforeSources | kCFRunLoopAfterWaiting, true, 0, &RunLoopObserverCallback, &context);
    CFRunLoopAddObserver(CFRunLoopGetMain(), observer, kCFRunLoopCommonModes);
    CFRelease(observer);
    
    [AppleDetail addUserInitiatedQueueOperation:^{
        while (true) {
            long result = dispatch_semaphore_wait(semaphore, dispatch_time(DISPATCH_TIME_NOW, (int64_t)(10 * NSEC_PER_SEC)));
            
            if (result == 0) {
                continue;
            }
            
            if (self.m_monitoringEnabled == NO) {
                dispatch_semaphore_signal(semaphore);
                continue;
            }
            
            [AppleFirebaseCrashlyticsANRMonitor backtraceOfMachPort:thread withName:name];
        }
    }];
}

static BOOL is_valid_frame_pointer( void ** _frame, uintptr_t _stackLow, uintptr_t _stackHigh ) {
    if( _frame == NULL )
    {
        return NO;
    }
    
    uintptr_t frameValue = (uintptr_t)_frame;
    
    if( frameValue < _stackLow || frameValue > _stackHigh )
    {
        return NO;
    }
    
    if( (frameValue & 0xF) != 0 )
    {
        return NO;
    }
    
    return YES;
}

static size_t my_backtrace_from_fp( _STRUCT_ARM_THREAD_STATE64 _state, void ** _buffer, size_t _size, uintptr_t _stackLow, uintptr_t _stackHigh ) {
    void **fp = (void **)(uintptr_t)_state.__fp;
    void *pc = (void *)(uintptr_t)_state.__pc;
    
    if (is_valid_frame_pointer(fp, _stackLow, _stackHigh) == NO || pc == NULL) {
        return 0;
    }
    
    size_t i = 0;
    
    _buffer[i++] = pc;
    
    while (fp && i < _size) {
        if (is_valid_frame_pointer(fp, _stackLow, _stackHigh) == NO) {
            break;
        }
        
        void *saved_pc = *(fp + 1);
        
        if (saved_pc == NULL) break;
        
        _buffer[i++] = saved_pc;
        
        void **next_fp = (void **)(*fp);
        
        if (next_fp <= fp || is_valid_frame_pointer(next_fp, _stackLow, _stackHigh) == NO) {
            break;
        }
        
        fp = next_fp;
    }
    
    return i;
}

+ (BOOL)backtraceOfMachPort:(pthread_t)thread withName:(NSString *)name {
#if defined(__aarch64__)
    mach_port_t mach_port = pthread_mach_thread_np(thread);
    
    _STRUCT_ARM_THREAD_STATE64 state = {0};
    mach_msg_type_number_t count = ARM_THREAD_STATE64_COUNT;
    kern_return_t kr = thread_get_state(mach_port, ARM_THREAD_STATE64, (thread_state_t)&state, &count);
    if (kr != KERN_SUCCESS) {
        return NO;
    }
    
    void * buffer[128] = {NULL};
    
    uintptr_t stackLow = 0;
    uintptr_t stackHigh = 0;
    
    // Try to get stack bounds via pthread API
    void * stackAddr = pthread_get_stackaddr_np(thread);
    size_t stackSize = pthread_get_stacksize_np(thread);
    
    if( stackAddr != NULL && stackSize > 0 )
    {
        stackHigh = (uintptr_t)stackAddr;
        stackLow = stackHigh - (uintptr_t)stackSize;
    }
    else
    {
        // Fallback: use heuristic address range for user-space stack
        // Typical user-space stack addresses on 64-bit iOS
        stackLow = 0x100000000ULL;
        stackHigh = 0x7FFFFFFFFFFFF000ULL;
    }
    
    size_t countFrames = my_backtrace_from_fp(state, buffer, 128, stackLow, stackHigh);
    
    if (countFrames == 0) {
        return NO;
    }
    
#if defined(MENGINE_DEBUG)
    char **symbols = backtrace_symbols(buffer, (int)countFrames);
    NSMutableArray<NSString *> *callStackSymbols = [NSMutableArray arrayWithCapacity:countFrames];
    for (int i = 0; i < countFrames; ++i) {
        if (symbols[i]) {
            [callStackSymbols addObject:[NSString stringWithUTF8String:symbols[i]]];
        } else {
            [callStackSymbols addObject:@"<null>"];
        }
    }
    free(symbols);
    
    IOS_LOGGER_MESSAGE(@"[ANR] detected on %@ thread: %@", name, callStackSymbols);
#endif
    
    FIRExceptionModel *exceptionModel = [[FIRExceptionModel alloc] initWithName:@"ANRException"
                                                                         reason:[NSString stringWithFormat:@"ANR detected on %@ thread", name]];
    
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

@end
