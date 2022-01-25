#import "AppleAppTrackingNative.h"

#import "AppleAppTrackingPlugin-Swift.h"

void AppleAppTrackingAuthorization()
{
	[AppleAppTrackingNative requestAuthorization];
}
