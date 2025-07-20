#pragma once

#import "Environment/Apple/AppleIncluder.h"
#include "Environment/Python/PythonIncluder.h"

@interface ApplePythonProvider : NSObject

- (instancetype)initWithCbs:(const pybind::dict &)cbs args:(const pybind::args &)args;
- (pybind::object)getMethod:(NSString *)method;

@property (nonatomic, assign) pybind::dict m_cbs;
@property (nonatomic, assign) pybind::args m_args;
@end
