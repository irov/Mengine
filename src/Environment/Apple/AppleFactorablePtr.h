#pragma once

#import "Environment/Apple/AppleIncluder.h"

#include "Kernel/Factorable.h"

@interface AppleFactorablePtr : NSObject

- (instancetype)initWithValue:(const Mengine::FactorablePtr &)value;
- (const Mengine::FactorablePtr &)value;

@property (nonatomic, assign) Mengine::FactorablePtr value;
@end
