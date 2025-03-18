#import "AppleAdvertisementAttempts.h"

@implementation AppleAdvertisementAttempts

- (instancetype)init {
    self = [super init];
    
    self.m_attempts = 0;
    
    return self;
}

- (NSInteger)getAttempts {
    return self.m_attempts;
}

- (NSInteger)incrementAttempts {
    self.m_attempts++;
    
    return self.m_attempts;
}

@end
