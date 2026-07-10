#include "ImGUIMetalBuffer.h"

#import <time.h>

//////////////////////////////////////////////////////////////////////////
static double MengineImGUIMetalBufferCurrentTime()
{
    return (double)clock_gettime_nsec_np( CLOCK_UPTIME_RAW ) / 1e9;
}
//////////////////////////////////////////////////////////////////////////
@implementation MengineImGUIMetalBuffer

- (instancetype)initWithBuffer:(id<MTLBuffer>)_buffer
{
    self = [super init];

    if( self == nil )
    {
        return nil;
    }

    self.buffer = _buffer;
    self.lastReuseTime = MengineImGUIMetalBufferCurrentTime();

    return self;
}

@end
