#pragma once

#include "iOSHelpshiftInterface.h"

#import <Foundation/Foundation.h>
#import <HelpshiftX/Helpshift.h>

@interface iOSHelpshiftDelegate : NSObject<HelpshiftDelegate>

@property (nonatomic, weak) id<iOSHelpshiftInterface> m_helpshift;

- (instancetype _Nonnull) initWithHelpshift:(id<iOSHelpshiftInterface>)helpshift;

- (NSString * _Nonnull) convertToString:(NSObject * _Nullable) object;

@end
