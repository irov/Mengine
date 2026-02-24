#pragma once

#import "Environment/Apple/AppleIncluder.h"

#include "Environment/Python/PythonIncluder.h"

@interface AppleNativePythonCallbackDesc : NSObject {
@public
    pybind::object m_cb;
    pybind::args m_args;
}

- (instancetype)initWithCallback:(const pybind::object &)cb
                            args:(const pybind::args &)args;

@end
