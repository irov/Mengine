#import "AppleAdvertisementCooldown.h"

#import "Environment/Apple/AppleDetail.h"

@implementation AppleAdvertisementCooldown

- (instancetype)init {
    self = [super init];
    
    self.m_timestamp = 0.0;
    
    return self;
}

- (NSInteger)getLastShownTimestamp {
    return self.m_timestamp;
}

- (void)resetShownTimestamp {
    NSInteger timestamp = [AppleDetail getTimestamp];
    
    self.m_timestamp = timestamp;
}

@end
