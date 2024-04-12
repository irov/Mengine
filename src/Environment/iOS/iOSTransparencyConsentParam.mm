#import "iOSTransparencyConsentParam.h"

@implementation iOSTransparencyConsentParam

- (instancetype _Nonnull)initFromUserDefaults {
    self = [super init];

    self.TRANSPARENCYCONSENT_CMPSDKID = [NSUserDefaults.standardUserDefaults integerForKey:@"IABTCF_CmpSdkID"];
    self.TRANSPARENCYCONSENT_CMPSDKVERSION = [NSUserDefaults.standardUserDefaults integerForKey:@"IABTCF_CmpSdkVersion"];
    self.TRANSPARENCYCONSENT_POLICYVERSION = [NSUserDefaults.standardUserDefaults integerForKey:@"IABTCF_PolicyVersion"];
    self.TRANSPARENCYCONSENT_GDPRAPPLIES = [NSUserDefaults.standardUserDefaults integerForKey:@"IABTCF_gdprApplies"];
    self.TRANSPARENCYCONSENT_PUBLISHERCC = [NSUserDefaults.standardUserDefaults stringForKey:@"IABTCF_PublisherCC"] ?: @"AA";
    self.TRANSPARENCYCONSENT_PURPOSEONETREATMENT = [NSUserDefaults.standardUserDefaults integerForKey:@"IABTCF_PurposeOneTreatment"];
    self.TRANSPARENCYCONSENT_USENONSTANDARTTEXTS = [NSUserDefaults.standardUserDefaults integerForKey:@"IABTCF_UseNonStandardStacks"];
    self.TRANSPARENCYCONSENT_TCSTRING = [NSUserDefaults.standardUserDefaults stringForKey:@"IABTCF_TCString"] ?: @"";
    self.TRANSPARENCYCONSENT_VENDORCONSENTS = [NSUserDefaults.standardUserDefaults stringForKey:@"IABTCF_VendorConsents"] ?: @"";
    self.TRANSPARENCYCONSENT_VEMDORLEGITIMATEINTERESTS = [NSUserDefaults.standardUserDefaults stringForKey:@"IABTCF_VendorLegitimateInterests"] ?: @"";
    self.TRANSPARENCYCONSENT_PURPOSECONSENTS = [NSUserDefaults.standardUserDefaults stringForKey:@"IABTCF_PurposeConsents"] ?: @"";
    self.TRANSPARENCYCONSENT_PURPOSELEGITIMATEINTERESTS = [NSUserDefaults.standardUserDefaults stringForKey:@"IABTCF_PurposeLegitimateInterests"] ?: @"";
    self.TRANSPARENCYCONSENT_SPECIALFEATURESOPTINS = [NSUserDefaults.standardUserDefaults stringForKey:@"IABTCF_SpecialFeaturesOptins"] ?: @"";
    self.TRANSPARENCYCONSENT_PUBLISHERRESTRICTIONS = [NSUserDefaults.standardUserDefaults stringForKey:@"IABTCF_PublisherRestrictions"] ?: @"";
    self.TRANSPARENCYCONSENT_PUBLISHERCONSENTS = [NSUserDefaults.standardUserDefaults stringForKey:@"IABTCF_PublisherConsents"] ?: @"";
    self.TRANSPARENCYCONSENT_PUBLISHERLEGITIMATEINTERESTS = [NSUserDefaults.standardUserDefaults stringForKey:@"IABTCF_PublisherLegitimateInterests"] ?: @"";
    self.TRANSPARENCYCONSENT_PUBLISHERCUSTOMPURPOSESCONSENTS = [NSUserDefaults.standardUserDefaults stringForKey:@"IABTCF_PublisherCustomPurposesConsents"] ?: @"";
    self.TRANSPARENCYCONSENT_PUBLISHERCUSTOMPURPOSESLEGITIMATEINTERESTS = [NSUserDefaults.standardUserDefaults stringForKey:@"IABTCF_PublisherCustomPurposesLegitimateInterests"] ?: @"";
    
    return self;
}

- (BOOL)getPurposeConsentArgument:(NSInteger)index {
    if (self.TRANSPARENCYCONSENT_GDPRAPPLIES == 0) {
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

- (BOOL)isEEA {
    if (self.TRANSPARENCYCONSENT_GDPRAPPLIES == 0) {
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
