#pragma once

#include "AppleHelpshiftInterface.h"

#import <Foundation/Foundation.h>
#import <HelpshiftX/Helpshift.h>

@interface AppleHelpshiftDelegate : NSObject<HelpshiftDelegate>

@property (assign) Mengine::AppleHelpshiftServiceInterface * _Nonnull m_service;

- (instancetype _Nonnull) initWithService:(Mengine::AppleHelpshiftServiceInterface* _Nonnull) service;

- (const char *) convertToChar:(NSObject * _Nullable) object;

@end
