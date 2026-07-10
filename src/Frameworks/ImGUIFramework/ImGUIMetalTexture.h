#pragma once

#import <Foundation/Foundation.h>
#import <Metal/Metal.h>

@interface MengineImGUIMetalTexture : NSObject

@property(nonatomic, strong) id<MTLTexture> texture;

- (instancetype)initWithTexture:(id<MTLTexture>)_texture;

@end
