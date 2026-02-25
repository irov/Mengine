#import "ApplePythonProvider.h"

@implementation ApplePythonProvider

- (instancetype)initWithCbs:(const pybind::dict &)cbs args:(const pybind::args &)args {
    self = [super init];
    
    self.m_cbs = cbs;
    self.m_args = args;
    
    return self;
}

- (pybind::object)getMethod:(NSString *)method {
    const char * method_str = [method UTF8String];
    
    pybind::object py_cb = self.m_cbs[method_str];
    
    return py_cb;
}

@end
