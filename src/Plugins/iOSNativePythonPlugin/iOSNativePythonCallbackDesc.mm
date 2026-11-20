#import "iOSNativePythonCallbackDesc.h"

@implementation iOSNativePythonCallbackDesc

- (instancetype)initWithCallback:(const pybind::object &)cb
                            args:(const pybind::args &)args {
    self = [super init];

    if (self == nil) {
        return nil;
    }

    m_cb = cb;
    m_args = args;

    return self;
}

@end
