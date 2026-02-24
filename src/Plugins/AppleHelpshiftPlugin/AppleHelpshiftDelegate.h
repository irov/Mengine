#pragma once

#include "AppleHelpshiftInterface.h"

#import <Foundation/Foundation.h>
#import <HelpshiftX/Helpshift.h>

@interface AppleHelpshiftDelegate : NSObject<HelpshiftDelegate>

@property (nonatomic, weak) id<AppleHelpshiftInterface> m_helpshift;

- (instancetype _Nonnull) initWithHelpshift:(id<AppleHelpshiftInterface>)helpshift;

- (NSString * _Nonnull) convertToString:(NSObject * _Nullable) object;

@end
