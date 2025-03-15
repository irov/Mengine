#include "Config/Config.h"

#import <Foundation/Foundation.h>

@protocol iOSPluginConfigDelegateInterface <NSObject>

- (void)onConfig:(NSDictionary * _Nonnull)config;

@end
