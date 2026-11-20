#include "AppleFactorablePtr.h"

@implementation AppleFactorablePtr

- (instancetype)initWithValue:(const Mengine::FactorablePtr &)value {
    self = [super init];

    if (self == nil) {
        return nil;
    }

    self.value = value;
    
    return self;
}

- (const Mengine::FactorablePtr &)value {
    return self.value;
}

@end