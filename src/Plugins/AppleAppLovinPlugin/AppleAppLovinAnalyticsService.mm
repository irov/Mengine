#import "AppleAppLovinAnalyticsService.h"

@implementation AppleAppLovinAnalyticsService

- (instancetype _Nonnull) init {
    self.m_providers = [NSMutableArray array];
    
    return self;
}

- (void) addAnalitycsDelegate:(id<AppleAppLovinAnalyticsDelegate> _Nonnull) provider {
    [self.m_providers addObject: provider];
}

- (void) eventRevenuePaid:(MAAd * _Nonnull) ad {
    for (id<AppleAppLovinAnalyticsDelegate> provider in self.m_providers) {
        [provider eventRevenuePaid:ad];
    }
}

@end
