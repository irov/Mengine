#pragma once

#include "Environment/Python/PythonIncluder.h"

#import <Foundation/Foundation.h>

@interface ApplePythonProvider : NSObject

- (instancetype)initWithCbs:(const pybind::dict &)cbs args:(const pybind::args &)args;
- (const pybind::object &)getMethod:(NSString *)method;

@property (nonatomic, assign) pybind::dict m_cbs;
@property (nonatomic, assign) pybind::args m_args;
@end
