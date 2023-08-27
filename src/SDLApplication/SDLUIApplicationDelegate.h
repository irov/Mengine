#import <UIKit/UIKit.h>

@interface SDLUIApplicationDelegate : NSObject<UIApplicationDelegate>

@property (nonatomic, strong) NSMutableArray<id> * m_pluginDelegates;

@end
