#pragma once

#import "Environment/iOS/iOSUIMainApplicationDelegateInterface.h"

#import "Environment/iOS/iOSPluginApplicationDelegateInterface.h"
#import "Environment/iOS/iOSPluginLoggerDelegateInterface.h"
#import "Environment/iOS/iOSPluginConfigDelegateInterface.h"
#import "Environment/iOS/iOSPluginAnalyticDelegateInterface.h"
#import "Environment/iOS/iOSPluginUserIdDelegateInterface.h"
#import "Environment/iOS/iOSPluginAdRevenueDelegateInterface.h"
#import "Environment/iOS/iOSPluginAppTrackingTransparencyDelegateInterface.h"
#import "Environment/iOS/iOSPluginTransparencyConsentDelegateInterface.h"

#import <UIKit/UIKit.h>

@interface iOSUIApplicationDelegate : NSObject<UIApplicationDelegate, iOSUIMainApplicationDelegateInterface>

@property (nonatomic, strong) UIWindow * m_window;

@property (nonatomic, strong) NSMutableArray<id> * m_pluginDelegates;
@property (nonatomic, strong) NSMutableArray<iOSPluginApplicationDelegateInterface> * m_pluginApplicationDelegates;
@property (nonatomic, strong) NSMutableArray<iOSPluginLoggerDelegateInterface> * m_pluginLoggerDelegates;
@property (nonatomic, strong) NSMutableArray<iOSPluginConfigDelegateInterface> * m_pluginConfigDelegates;
@property (nonatomic, strong) NSMutableArray<iOSPluginAnalyticDelegateInterface> * m_pluginAnalyticDelegates;
@property (nonatomic, strong) NSMutableArray<iOSPluginUserIdDelegateInterface> * m_pluginUserIdDelegates;
@property (nonatomic, strong) NSMutableArray<iOSPluginAdRevenueDelegateInterface> * m_pluginAdRevenueDelegates;
@property (nonatomic, strong) NSMutableArray<iOSPluginAppTrackingTransparencyDelegateInterface> * m_pluginAppTrackingTransparencyDelegates;
@property (nonatomic, strong) NSMutableArray<iOSPluginTransparencyConsentDelegateInterface> * m_pluginTransparencyConsentDelegates;

@end
