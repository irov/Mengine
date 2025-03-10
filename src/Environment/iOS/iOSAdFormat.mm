#import "iOSAdFormat.h"

#define DECLARE_IOS_ADFORMAT(NAME)\
    + (instancetype)NAME {\
        static iOSAdFormat * format = nil;\
        static dispatch_once_t onceToken;\
        dispatch_once(&onceToken, ^{\
            format = [[iOSAdFormat alloc] initWithName:@(#NAME)];\
        });\
        return format;\
    }

@implementation iOSAdFormat

DECLARE_IOS_ADFORMAT(ADFORMAT_BANNER)
DECLARE_IOS_ADFORMAT(ADFORMAT_MREC)
DECLARE_IOS_ADFORMAT(ADFORMAT_INTERSTITIAL)
DECLARE_IOS_ADFORMAT(ADFORMAT_REWARDED)
DECLARE_IOS_ADFORMAT(ADFORMAT_APPOPEN)

- (instancetype)initWithName:(NSString *)name {
    self = [super init];
    if (self) {
        _name = [name copy];
    }
    return self;
}

- (NSString *)description {
    return self.name;
}

@end
