#import <UIKit/UIKit.h>

#import "Environment/iOS/UIMainApplicationDelegateInterface.h"

@interface SDLUIApplicationDelegate : NSObject<UIApplicationDelegate, UIMainApplicationDelegateInterface>

@property (nonatomic, strong) NSMutableArray<UIProxyApplicationDelegateInterface> * m_pluginDelegates;

@end
