#import "AppleAppLovinAnalyticsDelegate.h"

@interface AppleAppLovinFirebaseAnalytics : NSObject<AppleAppLovinAnalyticsDelegate>

- (void) eventRevenuePaid:(MAAd * _Nonnull) ad;

@end
