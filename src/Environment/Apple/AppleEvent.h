#pragma once

#include "Config/Config.h"

#import <Foundation/Foundation.h>

@interface MengineEvent : NSObject

@property (class, nonatomic, readonly) MengineEvent *EVENT_GDPR_PASS;
@property (class, nonatomic, readonly) MengineEvent *EVENT_PUSH_TOKEN;
@property (class, nonatomic, readonly) MengineEvent *EVENT_ADVERTISING_ID;
@property (class, nonatomic, readonly) MengineEvent *EVENT_SESSION_ID;
@property (class, nonatomic, readonly) MengineEvent *EVENT_REMOTE_CONFIG_FETCH;
@property (class, nonatomic, readonly) MengineEvent *EVENT_TRANSPARENCY_CONSENT;

@property (nonatomic, strong, readonly) NSString *name;

- (instancetype)initWithName:(NSString *)name;
- (NSString *)getName;

@end