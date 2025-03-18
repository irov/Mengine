#import "AppleAdvertisementCooldown.h"

#import "Environment/Apple/AppleDetail.h"

@implementation AppleAdvertisementCooldown

- (instancetype)init {
    self = [super init];
    
    self.m_timestamp = 0.0;
    
    return self;
}

- (NSTimeInterval)getLastShownTimestamp {
    return self.m_timestamp;
}

- (void)resetShownTimestamp {
    NSTimeInterval timestamp = [AppleDetail getTimestamp];
    
    self.m_timestamp = timestamp;
}

@end
