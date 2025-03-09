#pragma once

#include "Config/Config.h"

#import <Foundation/Foundation.h>

@interface iOSAdFormat : NSObject

@property( nonatomic, strong, readonly ) NSString * name;

+(instancetype)ADFORMAT_BANNER;
+(instancetype)ADFORMAT_MREC;
+(instancetype)ADFORMAT_INTERSTITIAL;
+(instancetype)ADFORMAT_REWARDED;
+(instancetype)ADFORMAT_APPOPEN;

-(instancetype)initWithName:(NSString *)name;

@end