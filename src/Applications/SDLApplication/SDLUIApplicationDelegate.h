#import <UIKit/UIKit.h>

#import "Environment/iOS/UIMainApplicationDelegateInterface.h"

#include "SDLApplication.h"

@interface SDLUIApplicationDelegate : NSObject<UIApplicationDelegate, UIMainApplicationDelegateInterface>

@property (nonatomic, strong) NSMutableArray<UIPluginApplicationDelegateInterface> * m_pluginDelegates;

@end
