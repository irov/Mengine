#pragma once

#import "Environment/Apple/AppleIncluder.h"

typedef NS_ENUM(NSInteger, iOSConsentFlowUserGeography)
{    
    /**
     * User's geography is unknown.
     */
    iOSConsentFlowUserGeographyUnknown,
    
    /**
     * The user is in GDPR region.
     */
    iOSConsentFlowUserGeographyGDPR,
    
    /**
     * The user is in a non-GDPR region.
     */
    iOSConsentFlowUserGeographyOther
};
