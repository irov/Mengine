#pragma once

#import <Foundation/Foundation.h>
#import <Metal/Metal.h>

@interface MengineImGUIMetalBuffer : NSObject

@property(nonatomic, strong) id<MTLBuffer> buffer;
@property(nonatomic, assign) double lastReuseTime;

- (instancetype)initWithBuffer:(id<MTLBuffer>)_buffer;

@end
