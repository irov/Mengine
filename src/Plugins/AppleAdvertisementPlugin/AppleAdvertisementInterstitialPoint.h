#pragma once

#include "Config/Config.h"

#import "AppleAdvertisementBasePoint.h"

#import <Foundation/Foundation.h>

@interface AppleAdvertisementInterstitialPoint : AppleAdvertisementBasePoint

@property (nonatomic) NSInteger m_actionOffset;
@property (nonatomic) NSInteger m_actionCooldown;
@property (nonatomic) NSTimeInterval m_timeOffset;
@property (nonatomic) NSTimeInterval m_timeCooldown;
@property (nonatomic) NSTimeInterval m_installTimeOffset;
@property (nonatomic) NSTimeInterval m_sessionOffset;

@end
