#pragma once

#include "Kernel/Factorable.h"

#import <Foundation/Foundation.h>

@interface AppleFactorablePtr : NSObject

- (instancetype)initWithValue:(const Mengine::FactorablePtr &)value;
- (const Mengine::FactorablePtr &)value;

@property (nonatomic, assign) Mengine::FactorablePtr value;
@end
