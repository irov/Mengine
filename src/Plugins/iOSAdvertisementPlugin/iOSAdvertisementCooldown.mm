#import "iOSAdvertisementCooldown.h"

#import "Environment/Apple/AppleDetail.h"
#import "Environment/Apple/AppleUserDefaults.h"

#import "Environment/iOS/iOSLog.h"

@implementation iOSAdvertisementCooldown {
    NSString * m_preferenceKey;
    NSInteger m_timestamp;
}

- (instancetype)initWithPreferenceKey:(NSString *)_preferenceKey {
    self = [super init];

    if (self == nil) {
        return nil;
    }

    m_preferenceKey = _preferenceKey;
    m_timestamp = 0;

    if (m_preferenceKey == nil) {
        return self;
    }

    m_timestamp = [AppleUserDefaults getIntegerForKey:m_preferenceKey defaultValue:0];

    return self;
}

- (NSInteger)getLastShownTimestamp {
    @synchronized (self) {
        return m_timestamp;
    }
}

- (void)resetShownTimestamp {
    @synchronized (self) {
        m_timestamp = [AppleDetail getTimestamp];
    }
}

- (void)save {
    if (m_preferenceKey == nil) {
        return;
    }

    @synchronized (self) {
        if ([AppleUserDefaults setIntegerForKey:m_preferenceKey value:m_timestamp] == NO) {
            IOS_LOGGER_WARNING(@"failed to save cooldown '%@'", m_preferenceKey);
        }
    }
}

@end
