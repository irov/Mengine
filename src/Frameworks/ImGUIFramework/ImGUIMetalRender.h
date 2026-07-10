#pragma once

#include "Interface/RenderTextureInterface.h"

#include "imgui.h"

#import <Foundation/Foundation.h>

@interface MengineImGUIMetalRender : NSObject

+ (instancetype)sharedInstance;

+ (BOOL)initializeRenderer;
+ (void)finalizeRenderer;
+ (BOOL)isInitialized;

+ (void)newFrame;
+ (void)renderDrawData:(ImDrawData *)_drawData;
+ (ImTextureID)textureForRenderTexture:(const Mengine::RenderTextureInterfacePtr &)_texture;

@end
