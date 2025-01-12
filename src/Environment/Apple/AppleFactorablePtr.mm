#include "AppleFactorablePtr.h"

@implementation AppleFactorablePtr

- (instancetype)initWithValue:(const Mengine::FactorablePtr &)value {
    self = [super init];
    
    if (self) {
        self.value = value;
    }
    
    return self;
}

- (const Mengine::FactorablePtr &)value {
    return self.value;
}

@end