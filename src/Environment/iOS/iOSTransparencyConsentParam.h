#pragma once

#import "Environment/iOS/iOSConsentFlowUserGeography.h"

@interface iOSTransparencyConsentParam : NSObject

+ (void)setConsentFlowUserGeography:(iOSConsentFlowUserGeography)userGeography;
+ (iOSConsentFlowUserGeography)getConsentFlowUserGeography;

- (instancetype _Nonnull)initFromUserDefaults;

- (BOOL)isPending;
- (BOOL)getPurposeConsentArgument:(NSInteger)index;
- (BOOL)isEEA;
- (BOOL)getConsentAdStorage;
- (BOOL)getConsentAnalyticsStorage;
- (BOOL)getConsentAdPersonalization;
- (BOOL)getConsentAdUserData;

@property (nonatomic) BOOL TRANSPARENCYCONSENT_PENDING;
@property (nonatomic) NSInteger TRANSPARENCYCONSENT_CMPSDKID;
@property (nonatomic) NSInteger TRANSPARENCYCONSENT_CMPSDKVERSION;
@property (nonatomic) NSInteger TRANSPARENCYCONSENT_POLICYVERSION;
@property (nonatomic) NSInteger TRANSPARENCYCONSENT_GDPRAPPLIES;
@property (nonatomic, strong) NSString * _Nonnull TRANSPARENCYCONSENT_PUBLISHERCC;
@property (nonatomic) NSInteger TRANSPARENCYCONSENT_PURPOSEONETREATMENT;
@property (nonatomic) NSInteger TRANSPARENCYCONSENT_USENONSTANDARDSTACKS;
@property (nonatomic, strong) NSString * _Nonnull TRANSPARENCYCONSENT_TCSTRING;
@property (nonatomic, strong) NSString * _Nonnull TRANSPARENCYCONSENT_VENDORCONSENTS;
@property (nonatomic, strong) NSString * _Nonnull TRANSPARENCYCONSENT_VENDORLEGITIMATEINTERESTS;
@property (nonatomic, strong) NSString * _Nonnull TRANSPARENCYCONSENT_PURPOSECONSENTS;
@property (nonatomic, strong) NSString * _Nonnull TRANSPARENCYCONSENT_PURPOSELEGITIMATEINTERESTS;
@property (nonatomic, strong) NSString * _Nonnull TRANSPARENCYCONSENT_SPECIALFEATURESOPTINS;
@property (nonatomic, strong) NSString * _Nonnull TRANSPARENCYCONSENT_PUBLISHERRESTRICTIONS;
@property (nonatomic, strong) NSString * _Nonnull TRANSPARENCYCONSENT_PUBLISHERCONSENTS;
@property (nonatomic, strong) NSString * _Nonnull TRANSPARENCYCONSENT_PUBLISHERLEGITIMATEINTERESTS;
@property (nonatomic, strong) NSString * _Nonnull TRANSPARENCYCONSENT_PUBLISHERCUSTOMPURPOSESCONSENTS;
@property (nonatomic, strong) NSString * _Nonnull TRANSPARENCYCONSENT_PUBLISHERCUSTOMPURPOSESLEGITIMATEINTERESTS;

@end
