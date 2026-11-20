#import "iOSAdvertisementFrequency.h"

#import "Environment/Apple/AppleDetail.h"
#import "Environment/Apple/AppleUserDefaults.h"

@implementation iOSAdvertisementFrequency {
    NSString * m_showStartKey;
    NSString * m_showCountKey;
    NSInteger m_startTimestamp;
    NSInteger m_showCount;
}

- (instancetype)initWithPlacement:(NSString *)_placement {
    self = [super init];

    if (self == nil) {
        return nil;
    }

    NSString * frequencyKey = [@"ad.rewarded.frequency." stringByAppendingString:_placement];
    m_showStartKey = [frequencyKey stringByAppendingString:@".start"];
    m_showCountKey = [frequencyKey stringByAppendingString:@".count"];

    m_startTimestamp = [AppleUserDefaults getIntegerForKey:m_showStartKey defaultValue:0];
    m_showCount = [AppleUserDefaults getIntegerForKey:m_showCountKey defaultValue:0];

    return self;
}

- (BOOL)hasShowQuota:(NSInteger)_showLimit withTimeout:(NSInteger)_showTimeout {
    if (_showLimit < 0) {
        return YES;
    }

    @synchronized (self) {
        NSInteger count = m_showCount;
        NSInteger timestamp = [AppleDetail getTimestamp];

        if (timestamp - m_startTimestamp >= _showTimeout) {
            count = 0;
        }

        if (count >= _showLimit) {
            return NO;
        }

        return YES;
    }
}

- (void)recordShownWithTimeout:(NSInteger)_showTimeout {
    @synchronized (self) {
        NSInteger timestamp = [AppleDetail getTimestamp];

        if (m_showCount == 0 || timestamp - m_startTimestamp >= _showTimeout) {
            m_startTimestamp = timestamp;
            m_showCount = 0;
        }

        m_showCount += 1;
    }
}

- (void)save {
    @synchronized (self) {
        [AppleUserDefaults setIntegerForKey:m_showStartKey value:m_startTimestamp];
        [AppleUserDefaults setIntegerForKey:m_showCountKey value:m_showCount];
    }
}

@end
