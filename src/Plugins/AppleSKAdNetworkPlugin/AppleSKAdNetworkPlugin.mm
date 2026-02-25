#import "AppleSKAdNetworkPlugin.h"

#import "Environment/Apple/AppleDetail.h"
#import "Environment/Apple/AppleISO8601Date.h"
#import "Environment/Apple/AppleKeyChain.h"
#import "Environment/iOS/iOSLog.h"
#import "Environment/iOS/iOSAnalytics.h"
#import "Environment/iOS/iOSDetail.h"

#import "AppleSKAdNetworkDetail.h"

#import <StoreKit/SKAdNetwork.h>
#import <StoreKit/SKANError.h>

@implementation AppleSKAdNetworkPlugin

+ (instancetype)sharedInstance {
    static AppleSKAdNetworkPlugin * sharedInstance = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedInstance = [iOSDetail getPluginDelegateOfClass:[AppleSKAdNetworkPlugin class]];
    });
    return sharedInstance;
}

- (instancetype)init {
    self = [super init];
    
    if (self) {
        self.m_closed = NO;
        
        self.m_sendFine = -1;
        self.m_acceptFine = -1;
        self.m_time = 0.0;
        
        self.m_config = nil;
        self.m_adRevenue = 0.0;
    }
    
    return self;
}

#pragma mark - UIApplicationDelegate

- (BOOL)application:(UIApplication*)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    NSString * plistPath = [[NSBundle mainBundle] pathForResource:@MENGINE_APPLE_SKADNETWORK_ADREVENUE_PLIST_NAME ofType:@"plist"];

    NSDictionary * config = [NSDictionary dictionaryWithContentsOfFile:plistPath];
    
    if (config == nil) {
        IOS_LOGGER_ERROR( @"can't load skadnetwork adrevenue plist: %@"
            , @MENGINE_APPLE_SKADNETWORK_ADREVENUE_PLIST_NAME
        );
        
        return NO;
    }
    
#ifdef MENGINE_DEBUG
    if ([self validateSKANPlist:config] == NO) {
        IOS_LOGGER_ERROR( @"skadnetwork adrevenue plist is invalid: %@"
            , @MENGINE_APPLE_SKADNETWORK_ADREVENUE_PLIST_NAME
        );
        
        return NO;
    }
#endif
    
    self.m_config = config;
    
    NSInteger version = [AppleKeyChain getIntegerForKey:@"mengine.skadnetwork.version" defaultValue:1];
    BOOL closed = [AppleKeyChain getBooleanForKey:@"mengine.skadnetwork.closed" defaultValue:NO];
    NSInteger fine = [AppleKeyChain getIntegerForKey:@"mengine.skadnetwork.fine" defaultValue:-1];
    NSTimeInterval time = [AppleKeyChain getTimeIntervalForKey:@"mengine.skadnetwork.time" defaultValue:0.0];
    double adRevenue = [AppleKeyChain getDoubleForKey:@"mengine.skadnetwork.adrevenue" defaultValue:0.0];
    
    self.m_closed = closed;
    
    self.m_sendFine = fine;
    self.m_acceptFine = fine;
    
    self.m_time = time;
    self.m_adRevenue = adRevenue;
    
    [self recordInstallation];
    
    return YES;
}

- (BOOL)validateSKANPlist:(NSDictionary *)config {
    NSArray * windows = config[@"windows"];
        
    if ([windows isKindOfClass:[NSArray class]] == NO) {
        return NO;
    }
        
    NSMutableSet * windowIds = [NSMutableSet set];
    
    for (NSDictionary * window in windows) {
        if ([window isKindOfClass:[NSDictionary class]] == NO) {
            return NO;
        }
        
        NSString * windowId = window[@"id"];
                
        if ([windowId isKindOfClass:[NSString class]] == NO) {
            return NO;
        }
        
        if ([windowIds containsObject:windowId] == YES) {
            return NO;
        }
        
        [windowIds addObject:windowId];
        
        NSString * duration = window[@"duration"];
        
        if ([duration isKindOfClass:[NSString class]] == NO) {
            return NO;
        }
        
        NSTimeInterval interval = [AppleISO8601Date NSTimeIntervalFromISO8601Duration:duration];
        
        if (interval <= 0.0) {
            return NO;
        }
    }
    
    NSArray * conditions = config[@"conditions"];
    
    if ([conditions isKindOfClass:[NSArray class]] == NO) {
        return NO;
    }
    
    double previousThreshold = -1.0;
    NSInteger previousFine = -1;
    
    for (NSDictionary * condition in conditions) {
        if ([condition isKindOfClass:[NSDictionary class]] == NO) {
            return NO;
        }
        
        NSNumber * thresholdNumber = condition[@"threshold"];
        
        if ([thresholdNumber isKindOfClass:[NSNumber class]] == NO) {
            return NO;
        }
        
        double thresholdValue = thresholdNumber.doubleValue;
        
        if (thresholdValue <= previousThreshold) {
            return NO;
        }
        
        previousThreshold = thresholdValue;
        
        NSNumber * fineNumber = condition[@"fine"];
        
        if ([fineNumber isKindOfClass:[NSNumber class]] == NO) {
            return NO;
        }

        NSInteger fineValue = fineNumber.integerValue;
        
        if (fineValue <= previousFine) {
            return NO;
        }
        
        previousFine = fineValue;
        
        NSArray * coarses = condition[@"coarses"];
        
        if ([coarses isKindOfClass:[NSArray class]] == NO) {
            return NO;
        }
        
        if (coarses.count != windows.count) {
            return NO;
        }
        
        NSMutableSet * coarseWindows = [NSMutableSet set];
        
        for (NSDictionary * coarse in coarses) {
            if ([coarse isKindOfClass:[NSDictionary class]] == NO) {
                return NO;
            }
            
            NSString * coarseWindowId = coarse[@"window"];
            
            if ([coarseWindowId isKindOfClass:[NSString class]] == NO) {
                return NO;
            }

            if ([windowIds containsObject:coarseWindowId] == NO) {
                return NO;
            }
            
            if ([coarseWindows containsObject:coarseWindowId] == YES) {
                return NO;
            }
            
            [coarseWindows addObject:coarseWindowId];
            
            NSString * coarseValue = coarse[@"value"];
            
            if ([coarseValue isKindOfClass:[NSString class]] == NO) {
                return NO;
            }

            if ([@[@"low", @"medium", @"high"] containsObject:coarseValue] == NO) {
                return NO;
            }
        }
    }
    
    return YES;
}

- (void)recordInstallation {
    if (self.m_closed == YES) {
        return;
    }
    
    if (self.m_acceptFine >= 0) {
        return;
    }
    
    if (self.m_time > 0.0) {
        return;
    }
    
    @synchronized (self) {
        self.m_sendFine = 0;
    }
    
    [AppleSKAdNetworkDetail registerAppForAdNetworkAttribution:@"low"
                                           completionHandler:^(NSError *error) {
        if (error != nil) {
            IOS_LOGGER_ERROR( @"registerAppForAdNetworkAttribution error: %@"
                , [AppleDetail getMessageFromNSError:error]
            );
            
            [iOSAnalytics eventSystem:@"mng_skadnetwork_installation_error" params:@{
                @"error": error,
                @"error_code": @(error.code)
            }];
            
            return;
        }
        
        IOS_LOGGER_INFO( @"registerAppForAdNetworkAttribution" );
        
        @synchronized (self) {
            self.m_time = [[NSDate date] timeIntervalSince1970];
            self.m_acceptFine = 0;
        }
        
        [AppleKeyChain setTimeIntervalForKey:@"mengine.skadnetwork.time" value:self.m_time];
        [AppleKeyChain setIntegerForKey:@"mengine.skadnetwork.fine" value:0];
            
        [iOSAnalytics eventSystem:@"mng_skadnetwork_installation" params:@{}];
    }];
}

- (NSString *)getCurrentReportingWindow {
    NSTimeInterval current_time = [[NSDate date] timeIntervalSince1970];
    
    for(NSDictionary * window in self.m_config[@"windows"]) {
        NSString * duration_str = window[@"duration"];
        
        NSTimeInterval duration = [AppleISO8601Date NSTimeIntervalFromISO8601Duration:duration_str];
        
        if (self.m_time + duration < current_time) {
            continue;
        }
        
        NSString * window_id = window[@"id"];
            
        return window_id;
    }
    
    return nil;
}

- (NSArray *)getConversionValue:(NSString *)window_id withValue:(double)value {
    for(NSDictionary * condition in [self.m_config[@"conditions"] reverseObjectEnumerator]) {
        double threshold = [condition[@"threshold"] doubleValue];
        
        if (threshold > value) {
            continue;
        }
        
        NSNumber * fine = condition[@"fine"];
        
        for(NSDictionary * coarse in condition[@"coarses"]) {
            NSString * coarse_window_id = coarse[@"window"];
            
            if ([coarse_window_id isEqualToString:window_id] == NO) {
                continue;
            }
            
            NSString * coarse_value = coarse[@"value"];

            return @[fine, coarse_value];
        }
    }
    
    return nil;
}

#pragma mark - iOSPluginAdRevenueDelegateInterface

- (void)onAdRevenue:(iOSAdRevenueParam *)revenue {
    if (self.m_closed == YES) {
        return;
    }
    
    if (self.m_time == 0.0) {
        return;
    }
    
    double value = [revenue.REVENUE_VALUE doubleValue];
    
    if (value <= 0.0) {
        return;
    }
    
    NSString * window_id = [self getCurrentReportingWindow];
    
    if (window_id == nil) {
        self.m_closed = YES;
        
        [AppleKeyChain setBooleanForKey:@"mengine.skadnetwork.closed" value:YES];
        
        return;
    }
    
    double newValue = 0.0;
    
    @synchronized (self) {
        newValue = self.m_adRevenue + value;
        self.m_adRevenue = newValue;
    }
    
    [AppleKeyChain setDoubleForKey:@"mengine.skadnetwork.adrevenue" value:newValue];
    
    NSArray * conversionValue = [self getConversionValue:window_id withValue:newValue];
    
    if (conversionValue == nil) {
        return;
    }
    
    NSInteger fine = [conversionValue[0] integerValue];
    NSString * coarse = conversionValue[1];
    
    @synchronized (self) {
        if (self.m_acceptFine == fine) {
            return;
        } else if (self.m_acceptFine > fine) {
            IOS_LOGGER_ERROR( @"accept fine: %ld more fine: %ld coarse: %@"
                , self.m_acceptFine
                , fine
                , coarse
            );
            
            return;
        }
    
        if (self.m_sendFine == fine) {
            return;
        } else if (self.m_sendFine > fine) {
            IOS_LOGGER_ERROR( @"send fine: %ld more fine: %ld coarse: %@"
                , self.m_acceptFine
                , fine
                , coarse
            );
            
            return;
        }
        
        self.m_sendFine = fine;
    }

    [AppleSKAdNetworkDetail updatePostbackConversionValue:fine coarseValue:coarse lockWindow:NO completionHandler:^(NSError *error) {
        if (error != nil) {
            if ([AppleSKAdNetworkDetail isPostbackConversionUnknownError:error] == YES) {
                return;
            }
            
            IOS_LOGGER_ERROR( @"updatePostbackConversionValue fine: %ld coarse: %@ error: %@"
                , fine
                , coarse
                , [AppleDetail getMessageFromNSError:error]
            );
            
            [iOSAnalytics eventSystem:@"mng_skadnetwork_conversion_error" params:@{
                @"error": error,
                @"error_code": @(error.code),
                @"fine": @(fine),
                @"coarse": coarse,
                @"window": window_id,
                @"revenue": @(newValue)
            }];
            
            return;
        }
        
        IOS_LOGGER_INFO( @"updatePostbackConversionValue fine: %ld coarse: %@"
            , fine
            , coarse
        );
            
        @synchronized (self) {
            self.m_acceptFine = fine;
        }
        
        [AppleKeyChain setIntegerForKey:@"mengine.skadnetwork.fine" value:fine];
        
        [iOSAnalytics eventSystem:@"mng_skadnetwork_conversion" params:@{
            @"fine": @(fine),
            @"coarse": coarse,
            @"window": window_id,
            @"revenue": @(newValue)
        }];
    }];
}

@end
