#import "AppleNativePythonCallbackDesc.h"

@implementation AppleNativePythonCallbackDesc

- (instancetype)initWithCallback:(const pybind::object &)cb
                            args:(const pybind::args &)args {
    self = [super init];

    m_cb = cb;
    m_args = args;

    return self;
}

@end
