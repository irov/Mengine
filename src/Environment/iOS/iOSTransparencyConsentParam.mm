#import "iOSTransparencyConsentParam.h"

#import "Environment/iOS/iOSApplication.h"

@implementation iOSTransparencyConsentParam

static iOSConsentFlowUserGeography TRANSPARENCYCONSENT_USERGEOGRAPHY = iOSConsentFlowUserGeographyUnknown;

+ (void)setConsentFlowUserGeography:(iOSConsentFlowUserGeography)userGeography {
    if (TRANSPARENCYCONSENT_USERGEOGRAPHY == userGeography) {
        return;
    }
    
    TRANSPARENCYCONSENT_USERGEOGRAPHY = userGeography;
    
    [iOSApplication setPersistentIntegerForKey:@"mengine.consent.user_geography" value:userGeography];
}

+ (iOSConsentFlowUserGeography)getConsentFlowUserGeography {
    return TRANSPARENCYCONSENT_USERGEOGRAPHY;
}

+ (BOOL)computeTransparencyConsentPending {
    NSUserDefaults * defaults = NSUserDefaults.standardUserDefaults;
    
    if ([defaults objectForKey:@"IABTCF_gdprApplies"] == nil) {
        return YES;
    }

    if ([defaults objectForKey:@"IABTCF_TCString"] == nil) {
        return YES;
    }

    if ([defaults objectForKey:@"IABTCF_CmpSdkID"] == nil ||
        [defaults objectForKey:@"IABTCF_CmpSdkVersion"] == nil ||
        [defaults objectForKey:@"IABTCF_PolicyVersion"] == nil) {
        return YES;
    }

    return NO;
}

- (instancetype _Nonnull)initFromUserDefaults {
    self = [super init];
    
    NSUserDefaults * defaults = NSUserDefaults.standardUserDefaults;
    
    TRANSPARENCYCONSENT_USERGEOGRAPHY = (iOSConsentFlowUserGeography)[iOSApplication getPersistentIntegerForKey:@"mengine.consent.user_geography" defaultValue:iOSConsentFlowUserGeographyUnknown];

    self.TRANSPARENCYCONSENT_PENDING = [iOSTransparencyConsentParam computeTransparencyConsentPending];
    
    self.TRANSPARENCYCONSENT_CMPSDKID = [defaults integerForKey:@"IABTCF_CmpSdkID"];
    self.TRANSPARENCYCONSENT_CMPSDKVERSION = [defaults integerForKey:@"IABTCF_CmpSdkVersion"];
    self.TRANSPARENCYCONSENT_POLICYVERSION = [defaults integerForKey:@"IABTCF_PolicyVersion"];
    self.TRANSPARENCYCONSENT_GDPRAPPLIES = [defaults integerForKey:@"IABTCF_gdprApplies"];
    self.TRANSPARENCYCONSENT_PUBLISHERCC = [defaults stringForKey:@"IABTCF_PublisherCC"] ?: @"AA";
    self.TRANSPARENCYCONSENT_PURPOSEONETREATMENT = [defaults integerForKey:@"IABTCF_PurposeOneTreatment"];
    self.TRANSPARENCYCONSENT_USENONSTANDARTTEXTS = [defaults integerForKey:@"IABTCF_UseNonStandardStacks"];
    self.TRANSPARENCYCONSENT_TCSTRING = [defaults stringForKey:@"IABTCF_TCString"] ?: @"";
    self.TRANSPARENCYCONSENT_VENDORCONSENTS = [defaults stringForKey:@"IABTCF_VendorConsents"] ?: @"";
    self.TRANSPARENCYCONSENT_VEMDORLEGITIMATEINTERESTS = [defaults stringForKey:@"IABTCF_VendorLegitimateInterests"] ?: @"";
    self.TRANSPARENCYCONSENT_PURPOSECONSENTS = [defaults stringForKey:@"IABTCF_PurposeConsents"] ?: @"";
    self.TRANSPARENCYCONSENT_PURPOSELEGITIMATEINTERESTS = [defaults stringForKey:@"IABTCF_PurposeLegitimateInterests"] ?: @"";
    self.TRANSPARENCYCONSENT_SPECIALFEATURESOPTINS = [defaults stringForKey:@"IABTCF_SpecialFeaturesOptins"] ?: @"";
    self.TRANSPARENCYCONSENT_PUBLISHERRESTRICTIONS = [defaults stringForKey:@"IABTCF_PublisherRestrictions"] ?: @"";
    self.TRANSPARENCYCONSENT_PUBLISHERCONSENTS = [defaults stringForKey:@"IABTCF_PublisherConsents"] ?: @"";
    self.TRANSPARENCYCONSENT_PUBLISHERLEGITIMATEINTERESTS = [defaults stringForKey:@"IABTCF_PublisherLegitimateInterests"] ?: @"";
    self.TRANSPARENCYCONSENT_PUBLISHERCUSTOMPURPOSESCONSENTS = [defaults stringForKey:@"IABTCF_PublisherCustomPurposesConsents"] ?: @"";
    self.TRANSPARENCYCONSENT_PUBLISHERCUSTOMPURPOSESLEGITIMATEINTERESTS = [defaults stringForKey:@"IABTCF_PublisherCustomPurposesLegitimateInterests"] ?: @"";
    
    return self;
}

- (BOOL)isPending {
    if ([iOSTransparencyConsentParam getConsentFlowUserGeography] == iOSConsentFlowUserGeographyOther) {
        return NO;
    }
    
    return self.TRANSPARENCYCONSENT_PENDING;
}

- (BOOL)isEEA {
    if ([iOSTransparencyConsentParam getConsentFlowUserGeography] == iOSConsentFlowUserGeographyOther) {
        return NO;
    }
    
    if (self.TRANSPARENCYCONSENT_GDPRAPPLIES == 0) {
        return NO;
    }

    return YES;
}

- (BOOL)getPurposeConsentArgument:(NSInteger)index {
    if ([self isEEA] == NO) {
        return YES;
    }

    if( self.TRANSPARENCYCONSENT_PURPOSECONSENTS.length <= index )
    {
        return NO;
    }
    
    if( [self.TRANSPARENCYCONSENT_PURPOSECONSENTS characterAtIndex:index] == '0' )
    {
        return NO;
    }
    
    return YES;
}

- (BOOL)getConsentAdStorage {
    if ([self isEEA] == NO) {
        return YES;
    }

    if ([self getPurposeConsentArgument:0] == NO) {
        return NO;
    }

    return YES;
}

- (BOOL)getConsentAnalyticsStorage {
    if ([self isEEA] == NO) {
        return YES;
    }

    //ToDo

    return YES;
}

- (BOOL)getConsentAdPersonalization {
    if ([self isEEA] == NO) {
        return YES;
    }

    if ([self getPurposeConsentArgument:2] == NO) {
        return NO;
    }

    if ([self getPurposeConsentArgument:3] == NO) {
        return NO;
    }

    return YES;
}

- (BOOL)getConsentAdUserData {
    if ([self isEEA] == NO) {
        return YES;
    }

    if ([self getPurposeConsentArgument:0] == NO) {
        return NO;
    }

    if ([self getPurposeConsentArgument:6] == NO) {
        return NO;
    }

    return YES;
}

@end
