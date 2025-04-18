#import "AppleEvent.h"

@implementation AppleEvent

#define DECLARE_APPLE_EVENT(NAME)\
    + (AppleEvent *)NAME {\
        static AppleEvent *event = nil;\
        static dispatch_once_t onceToken;\
        dispatch_once(&onceToken, ^{\
            event = [[AppleEvent alloc] initWithName:@(#NAME)];\
        });\
        return event;\
    }

DECLARE_APPLE_EVENT(EVENT_GDPR_PASS)
DECLARE_APPLE_EVENT(EVENT_PUSH_TOKEN)
DECLARE_APPLE_EVENT(EVENT_ADVERTISING_ID)

- (instancetype)initWithName:(NSString *)name {
    self = [super init];
    if (self) {
        _name = name;
    }
    return self;
}

- (NSString *)getName {
    return _name;
}

@end
