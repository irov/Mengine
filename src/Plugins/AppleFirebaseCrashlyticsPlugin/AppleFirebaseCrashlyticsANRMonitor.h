#import <Foundation/Foundation.h>

@interface AppleFirebaseCrashlyticsANRMonitor : NSObject
@property( nonatomic, assign ) CFRunLoopObserverRef observer;
@end