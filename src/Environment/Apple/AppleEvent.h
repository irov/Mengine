#pragma once

#include "Config/Config.h"

#import <Foundation/Foundation.h>

@interface AppleEvent : NSObject

@property (class, nonatomic, readonly) AppleEvent * EVENT_GDPR_PASS;
@property (class, nonatomic, readonly) AppleEvent * EVENT_PUSH_TOKEN;
@property (class, nonatomic, readonly) AppleEvent * EVENT_ADVERTISING_ID;
@property (class, nonatomic, readonly) AppleEvent * EVENT_SESSION_ID;
@property (class, nonatomic, readonly) AppleEvent * EVENT_REMOTE_CONFIG_FETCH;

@property (nonatomic, strong, readonly) NSString * name;

- (instancetype)initWithName:(NSString *)name;
- (NSString *)getName;

@end
