#pragma once

#import "Environment/iOS/iOSUIMainApplicationDelegateInterface.h"

#import "Environment/iOS/iOSPluginApplicationDelegateInterface.h"
#import "Environment/iOS/iOSPluginAdRevenueDelegateInterface.h"

#import <UIKit/UIKit.h>

@interface SDLUIApplicationDelegate : NSObject<UIApplicationDelegate, iOSUIMainApplicationDelegateInterface>

@property (nonatomic, strong) NSMutableArray<iOSPluginApplicationDelegateInterface> * m_pluginApplicationDelegates;
@property (nonatomic, strong) NSMutableArray<iOSPluginAdRevenueDelegateInterface> * m_pluginAdRevenueDelegates;

@end
