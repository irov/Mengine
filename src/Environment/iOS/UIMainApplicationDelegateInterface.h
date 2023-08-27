#include "Config/Config.h"

#import <Foundation/Foundation.h>

@protocol UIMainApplicationDelegateInterface <NSObject>

- (void)notify:(NSString *)id args:(NSArray *)args;

@end
