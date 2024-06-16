#import "AppleEvent.h"

@implementation MengineEvent

+ (MengineEvent *)EVENT_GDPR_PASS {
    static MengineEvent *event = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        event = [[MengineEvent alloc] initWithName:@"EVENT_GDPR_PASS"];
    });
    return event;
}

+ (MengineEvent *)EVENT_PUSH_TOKEN {
    static MengineEvent *event = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        event = [[MengineEvent alloc] initWithName:@"EVENT_PUSH_TOKEN"];
    });
    return event;
}

+ (MengineEvent *)EVENT_ADVERTISING_ID {
    static MengineEvent *event = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        event = [[MengineEvent alloc] initWithName:@"EVENT_ADVERTISING_ID"];
    });
    return event;
}

+ (MengineEvent *)EVENT_SESSION_ID {
    static MengineEvent *event = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        event = [[MengineEvent alloc] initWithName:@"EVENT_SESSION_ID"];
    });
    return event;
}

+ (MengineEvent *)EVENT_REMOTE_CONFIG_FETCH {
    static MengineEvent *event = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        event = [[MengineEvent alloc] initWithName:@"EVENT_REMOTE_CONFIG_FETCH"];
    });
    return event;
}

+ (MengineEvent *)EVENT_TRANSPARENCY_CONSENT {
    static MengineEvent *event = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        event = [[MengineEvent alloc] initWithName:@"EVENT_TRANSPARENCY_CONSENT"];
    });
    return event;
}

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