#import "AppleAppTrackingNative.h"

#import "IOSImpl-Swift.h"

void AppleAppTrackingAuthorization()
{
	[AppTracking requestAuthorization];
}