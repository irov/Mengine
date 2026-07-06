#pragma once

#import "Environment/Apple/AppleIncluder.h"

typedef NS_ENUM( NSInteger, EiOSAppTrackingAuthorization )
{
    EAATA_AUTHORIZED,
    EAATA_DENIED,
    EAATA_RESTRICTED,
    EAATA_NOT_DETERMINED
};
