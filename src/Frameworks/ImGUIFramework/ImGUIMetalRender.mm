#include "ImGUIMetalRender.h"

#if defined(MENGINE_ENVIRONMENT_RENDER_METAL)

#include "ImGUIFramebufferDescriptor.h"
#include "ImGUIMetalBuffer.h"
#include "ImGUIMetalContext.h"
#include "ImGUIMetalTexture.h"

#import "Environment/Metal/MetalRenderImageExtensionInterface.h"
#import "Environment/Metal/MetalRenderSystemExtensionInterface.h"

#include "Interface/RenderSystemInterface.h"

#include "Kernel/Logger.h"

#import <Metal/Metal.h>
#import <TargetConditionals.h>
#import <dispatch/dispatch.h>

#include <cstring>

@interface MengineImGUIMetalRender()
{
    MengineImGUIMetalContext * m_context;
    id<MTLRenderCommandEncoder> m_renderCommandEncoder;
}

+ (MengineImGUIMetalRender *)currentRenderer;
+ (void)drawCallbackResetRenderStateWithDrawList:(const ImDrawList *)_drawList command:(const ImDrawCmd *)_command;
+ (void)drawCallbackSetSamplerLinearWithDrawList:(const ImDrawList *)_drawList command:(const ImDrawCmd *)_command;
+ (void)drawCallbackSetSamplerNearestWithDrawList:(const ImDrawList *)_drawList command:(const ImDrawCmd *)_command;

- (BOOL)initializeRenderer;
- (void)finalizeRenderer;
- (BOOL)isInitialized;
- (void)newFrame;
- (void)renderDrawData:(ImDrawData *)_drawData;
- (ImTextureID)textureForRenderTexture:(const Mengine::RenderTextureInterfacePtr &)_texture;
- (void)destroyTexture:(ImTextureData *)_texture;
- (void)updateTexture:(ImTextureData *)_texture;
- (BOOL)createDeviceObjects;
- (void)destroyDeviceObjects;
- (void)setupRenderState:(ImDrawData *)_drawData pipelineState:(id<MTLRenderPipelineState>)_pipelineState vertexBuffer:(MengineImGUIMetalBuffer *)_vertexBuffer vertexBufferOffset:(size_t)_vertexBufferOffset;
- (void)setFragmentSamplerLinear;
- (void)setFragmentSamplerNearest;

@end

static void MengineImGUIMetalRenderDrawCallbackResetRenderState( const ImDrawList * _drawList, const ImDrawCmd * _command )
{
    [MengineImGUIMetalRender drawCallbackResetRenderStateWithDrawList:_drawList command:_command];
}
//////////////////////////////////////////////////////////////////////////
static void MengineImGUIMetalRenderDrawCallbackSetSamplerLinear( const ImDrawList * _drawList, const ImDrawCmd * _command )
{
    [MengineImGUIMetalRender drawCallbackSetSamplerLinearWithDrawList:_drawList command:_command];
}
//////////////////////////////////////////////////////////////////////////
static void MengineImGUIMetalRenderDrawCallbackSetSamplerNearest( const ImDrawList * _drawList, const ImDrawCmd * _command )
{
    [MengineImGUIMetalRender drawCallbackSetSamplerNearestWithDrawList:_drawList command:_command];
}
//////////////////////////////////////////////////////////////////////////
@implementation MengineImGUIMetalRender

+ (instancetype)sharedInstance
{
    static MengineImGUIMetalRender * instance = nil;
    static dispatch_once_t onceToken;

    dispatch_once( &onceToken, ^{
        instance = [[MengineImGUIMetalRender alloc] init];
    } );

    return instance;
}
//////////////////////////////////////////////////////////////////////////
- (instancetype)init
{
    self = [super init];

    if( self == nil )
    {
        return nil;
    }

    m_context = nil;
    m_renderCommandEncoder = nil;

    return self;
}
//////////////////////////////////////////////////////////////////////////
+ (MengineImGUIMetalRender *)currentRenderer
{
    if( ImGui::GetCurrentContext() == nullptr )
    {
        return nil;
    }

    return (__bridge MengineImGUIMetalRender *)ImGui::GetIO().BackendRendererUserData;
}
//////////////////////////////////////////////////////////////////////////
+ (BOOL)initializeRenderer
{
    return [[MengineImGUIMetalRender sharedInstance] initializeRenderer];
}
//////////////////////////////////////////////////////////////////////////
+ (void)finalizeRenderer
{
    [[MengineImGUIMetalRender sharedInstance] finalizeRenderer];
}
//////////////////////////////////////////////////////////////////////////
+ (BOOL)isInitialized
{
    return [[MengineImGUIMetalRender sharedInstance] isInitialized];
}
//////////////////////////////////////////////////////////////////////////
+ (void)newFrame
{
    [[MengineImGUIMetalRender sharedInstance] newFrame];
}
//////////////////////////////////////////////////////////////////////////
+ (void)renderDrawData:(ImDrawData *)_drawData
{
    [[MengineImGUIMetalRender sharedInstance] renderDrawData:_drawData];
}
//////////////////////////////////////////////////////////////////////////
+ (ImTextureID)textureForRenderTexture:(const Mengine::RenderTextureInterfacePtr &)_texture
{
    return [[MengineImGUIMetalRender sharedInstance] textureForRenderTexture:_texture];
}
//////////////////////////////////////////////////////////////////////////
+ (void)drawCallbackResetRenderStateWithDrawList:(const ImDrawList *)_drawList command:(const ImDrawCmd *)_command
{
    (void)_drawList;
    (void)_command;
}
//////////////////////////////////////////////////////////////////////////
+ (void)drawCallbackSetSamplerLinearWithDrawList:(const ImDrawList *)_drawList command:(const ImDrawCmd *)_command
{
    (void)_drawList;
    (void)_command;

    MengineImGUIMetalRender * renderer = [MengineImGUIMetalRender currentRenderer];

    if( renderer != nil )
    {
        [renderer setFragmentSamplerLinear];
    }
}
//////////////////////////////////////////////////////////////////////////
+ (void)drawCallbackSetSamplerNearestWithDrawList:(const ImDrawList *)_drawList command:(const ImDrawCmd *)_command
{
    (void)_drawList;
    (void)_command;

    MengineImGUIMetalRender * renderer = [MengineImGUIMetalRender currentRenderer];

    if( renderer != nil )
    {
        [renderer setFragmentSamplerNearest];
    }
}
//////////////////////////////////////////////////////////////////////////
- (BOOL)initializeRenderer
{
    Mengine::MetalRenderSystemExtensionInterface * extension = RENDER_SYSTEM()
        ->getUnknown();

    if( extension == nullptr )
    {
        LOGGER_ERROR( "invalid get MetalRenderSystemExtensionInterface" );

        return NO;
    }

    id<MTLDevice> device = extension->getMetalDevice();

    if( device == nil )
    {
        LOGGER_ERROR( "invalid get Metal device for ImGUI" );

        return NO;
    }

    ImGuiIO & io = ImGui::GetIO();
    ImGuiPlatformIO & platformIO = ImGui::GetPlatformIO();

    IMGUI_CHECKVERSION();
    IM_ASSERT( io.BackendRendererUserData == nullptr && "Already initialized a renderer backend" );

    m_context = [[MengineImGUIMetalContext alloc] init];
    m_context.device = device;
    m_renderCommandEncoder = nil;

    io.BackendRendererUserData = (__bridge void *)self;
    io.BackendRendererName = "mengine_imgui_metal";
    io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;
    io.BackendFlags |= ImGuiBackendFlags_RendererHasTextures;

    platformIO.DrawCallback_ResetRenderState = MengineImGUIMetalRenderDrawCallbackResetRenderState;
    platformIO.DrawCallback_SetSamplerLinear = MengineImGUIMetalRenderDrawCallbackSetSamplerLinear;
    platformIO.DrawCallback_SetSamplerNearest = MengineImGUIMetalRenderDrawCallbackSetSamplerNearest;

    return YES;
}
//////////////////////////////////////////////////////////////////////////
- (void)finalizeRenderer
{
    if( [self isInitialized] == NO )
    {
        return;
    }

    [self destroyDeviceObjects];

    m_context = nil;
    m_renderCommandEncoder = nil;

    ImGuiIO & io = ImGui::GetIO();
    ImGuiPlatformIO & platformIO = ImGui::GetPlatformIO();

    io.BackendRendererName = nullptr;
    io.BackendRendererUserData = nullptr;
    io.BackendFlags &= ~(ImGuiBackendFlags_RendererHasVtxOffset | ImGuiBackendFlags_RendererHasTextures);

    platformIO.ClearRendererHandlers();
}
//////////////////////////////////////////////////////////////////////////
- (BOOL)isInitialized
{
    if( ImGui::GetCurrentContext() == nullptr )
    {
        return NO;
    }

    return ImGui::GetIO().BackendRendererUserData == (__bridge void *)self;
}
//////////////////////////////////////////////////////////////////////////
- (void)newFrame
{
    if( m_context == nil )
    {
        return;
    }

    Mengine::MetalRenderSystemExtensionInterface * extension = RENDER_SYSTEM()
        ->getUnknown();

    if( extension == nullptr )
    {
        return;
    }

    MTLRenderPassDescriptor * renderPassDescriptor = extension->getMetalRenderPassDescriptor();

    if( renderPassDescriptor == nil )
    {
        return;
    }

    m_context.framebufferDescriptor = [[MengineImGUIFramebufferDescriptor alloc] initWithRenderPassDescriptor:renderPassDescriptor];

    if( m_context.depthStencilState == nil )
    {
        [self createDeviceObjects];
    }
}
//////////////////////////////////////////////////////////////////////////
- (void)renderDrawData:(ImDrawData *)_drawData
{
    if( m_context == nil || _drawData == nullptr )
    {
        return;
    }

    Mengine::MetalRenderSystemExtensionInterface * extension = RENDER_SYSTEM()
        ->getUnknown();

    if( extension == nullptr )
    {
        return;
    }

    id<MTLCommandBuffer> commandBuffer = extension->getMetalCommandBuffer();
    id<MTLRenderCommandEncoder> commandEncoder = extension->getMetalRenderCommandEncoder();

    if( commandBuffer == nil || commandEncoder == nil )
    {
        return;
    }

    int fbWidth = (int)(_drawData->DisplaySize.x * _drawData->FramebufferScale.x);
    int fbHeight = (int)(_drawData->DisplaySize.y * _drawData->FramebufferScale.y);

    if( fbWidth <= 0 || fbHeight <= 0 || _drawData->CmdLists.Size == 0 || _drawData->TotalVtxCount == 0 || _drawData->TotalIdxCount == 0 )
    {
        return;
    }

    if( _drawData->Textures != nullptr )
    {
        for( ImTextureData * texture : *_drawData->Textures )
        {
            if( texture->Status != ImTextureStatus_OK )
            {
                [self updateTexture:texture];
            }
        }
    }

    if( m_context.framebufferDescriptor == nil )
    {
        MTLRenderPassDescriptor * renderPassDescriptor = extension->getMetalRenderPassDescriptor();

        if( renderPassDescriptor == nil )
        {
            return;
        }

        m_context.framebufferDescriptor = [[MengineImGUIFramebufferDescriptor alloc] initWithRenderPassDescriptor:renderPassDescriptor];
    }

    if( m_context.depthStencilState == nil )
    {
        if( [self createDeviceObjects] == NO )
        {
            return;
        }
    }

    id<MTLRenderPipelineState> pipelineState = m_context.renderPipelineStateCache[m_context.framebufferDescriptor];

    if( pipelineState == nil )
    {
        pipelineState = [m_context renderPipelineStateForFramebufferDescriptor:m_context.framebufferDescriptor device:commandBuffer.device];

        if( pipelineState == nil )
        {
            return;
        }

        m_context.renderPipelineStateCache[m_context.framebufferDescriptor] = pipelineState;
    }

    size_t vertexBufferLength = (size_t)_drawData->TotalVtxCount * sizeof( ImDrawVert );
    size_t indexBufferLength = (size_t)_drawData->TotalIdxCount * sizeof( ImDrawIdx );

    MengineImGUIMetalBuffer * vertexBuffer = [m_context dequeueReusableBufferOfLength:vertexBufferLength device:commandBuffer.device];
    MengineImGUIMetalBuffer * indexBuffer = [m_context dequeueReusableBufferOfLength:indexBufferLength device:commandBuffer.device];

    if( vertexBuffer == nil || indexBuffer == nil )
    {
        return;
    }

    m_renderCommandEncoder = commandEncoder;
    [self setupRenderState:_drawData pipelineState:pipelineState vertexBuffer:vertexBuffer vertexBufferOffset:0];

    ImGuiPlatformIO & platformIO = ImGui::GetPlatformIO();
    ImVec2 clipOffset = _drawData->DisplayPos;
    ImVec2 clipScale = _drawData->FramebufferScale;

    size_t vertexBufferOffset = 0;
    size_t indexBufferOffset = 0;

    for( const ImDrawList * drawList : _drawData->CmdLists )
    {
        std::memcpy( (char *)vertexBuffer.buffer.contents + vertexBufferOffset, drawList->VtxBuffer.Data, (size_t)drawList->VtxBuffer.Size * sizeof( ImDrawVert ) );
        std::memcpy( (char *)indexBuffer.buffer.contents + indexBufferOffset, drawList->IdxBuffer.Data, (size_t)drawList->IdxBuffer.Size * sizeof( ImDrawIdx ) );

        for( int commandIndex = 0; commandIndex != drawList->CmdBuffer.Size; ++commandIndex )
        {
            const ImDrawCmd * command = &drawList->CmdBuffer[commandIndex];

            if( command->UserCallback != nullptr )
            {
                if( command->UserCallback == platformIO.DrawCallback_ResetRenderState )
                {
                    [self setupRenderState:_drawData pipelineState:pipelineState vertexBuffer:vertexBuffer vertexBufferOffset:vertexBufferOffset];
                }
                else
                {
                    command->UserCallback( drawList, command );
                }

                continue;
            }

            ImVec2 clipMin( (command->ClipRect.x - clipOffset.x) * clipScale.x, (command->ClipRect.y - clipOffset.y) * clipScale.y );
            ImVec2 clipMax( (command->ClipRect.z - clipOffset.x) * clipScale.x, (command->ClipRect.w - clipOffset.y) * clipScale.y );

            if( clipMin.x < 0.f )
            {
                clipMin.x = 0.f;
            }

            if( clipMin.y < 0.f )
            {
                clipMin.y = 0.f;
            }

            if( clipMax.x > (float)fbWidth )
            {
                clipMax.x = (float)fbWidth;
            }

            if( clipMax.y > (float)fbHeight )
            {
                clipMax.y = (float)fbHeight;
            }

            if( clipMax.x <= clipMin.x || clipMax.y <= clipMin.y || command->ElemCount == 0 )
            {
                continue;
            }

            MTLScissorRect scissorRect =
            {
                .x = (NSUInteger)clipMin.x,
                .y = (NSUInteger)clipMin.y,
                .width = (NSUInteger)(clipMax.x - clipMin.x),
                .height = (NSUInteger)(clipMax.y - clipMin.y)
            };
            [commandEncoder setScissorRect:scissorRect];

            ImTextureID textureId = command->GetTexID();

            if( textureId != ImTextureID_Invalid )
            {
                [commandEncoder setFragmentTexture:(__bridge id<MTLTexture>)(void *)(uintptr_t)textureId atIndex:0];
            }

            [commandEncoder setVertexBufferOffset:(vertexBufferOffset + command->VtxOffset * sizeof( ImDrawVert )) atIndex:0];
            [commandEncoder drawIndexedPrimitives:MTLPrimitiveTypeTriangle
                                       indexCount:command->ElemCount
                                        indexType:sizeof( ImDrawIdx ) == 2 ? MTLIndexTypeUInt16 : MTLIndexTypeUInt32
                                      indexBuffer:indexBuffer.buffer
                                indexBufferOffset:indexBufferOffset + command->IdxOffset * sizeof( ImDrawIdx )];
        }

        vertexBufferOffset += (size_t)drawList->VtxBuffer.Size * sizeof( ImDrawVert );
        indexBufferOffset += (size_t)drawList->IdxBuffer.Size * sizeof( ImDrawIdx );
    }

    MengineImGUIMetalContext * context = m_context;

    [commandBuffer addCompletedHandler:^(id<MTLCommandBuffer>)
    {
        @synchronized( context.bufferCacheLock )
        {
            [context.bufferCache addObject:vertexBuffer];
            [context.bufferCache addObject:indexBuffer];
        }
    }];

    m_renderCommandEncoder = nil;
}
//////////////////////////////////////////////////////////////////////////
- (ImTextureID)textureForRenderTexture:(const Mengine::RenderTextureInterfacePtr &)_texture
{
    const Mengine::RenderImageInterfacePtr & image = _texture->getImage();

    Mengine::MetalRenderImageExtensionInterface * extension = image->getUnknown();

    id<MTLTexture> texture = extension->getMetalTexture();

    return (ImTextureID)(uintptr_t)(__bridge void *)texture;
}
//////////////////////////////////////////////////////////////////////////
- (void)destroyTexture:(ImTextureData *)_texture
{
    MengineImGUIMetalTexture * backendTexture = (__bridge_transfer MengineImGUIMetalTexture *)_texture->BackendUserData;

    if( backendTexture != nil )
    {
        IM_ASSERT( backendTexture.texture == (__bridge id<MTLTexture>)(void *)(uintptr_t)_texture->TexID );
        backendTexture.texture = nil;

        _texture->SetTexID( ImTextureID_Invalid );
        _texture->BackendUserData = nullptr;
    }

    _texture->SetStatus( ImTextureStatus_Destroyed );
}
//////////////////////////////////////////////////////////////////////////
- (void)updateTexture:(ImTextureData *)_texture
{
    if( m_context == nil )
    {
        return;
    }

    if( _texture->Status == ImTextureStatus_WantCreate )
    {
        IM_ASSERT( _texture->TexID == ImTextureID_Invalid && _texture->BackendUserData == nullptr );
        IM_ASSERT( _texture->Format == ImTextureFormat_RGBA32 );

        MTLTextureDescriptor * textureDescriptor = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatRGBA8Unorm
                                                                                                      width:(NSUInteger)_texture->Width
                                                                                                     height:(NSUInteger)_texture->Height
                                                                                                  mipmapped:NO];
        textureDescriptor.usage = MTLTextureUsageShaderRead;

#if TARGET_OS_OSX || TARGET_OS_MACCATALYST
        textureDescriptor.storageMode = MTLStorageModeManaged;
#else
        textureDescriptor.storageMode = MTLStorageModeShared;
#endif

        id<MTLTexture> metalTexture = [m_context.device newTextureWithDescriptor:textureDescriptor];

        if( metalTexture == nil )
        {
            LOGGER_ERROR( "invalid create ImGUI Metal texture %dx%d"
                , _texture->Width
                , _texture->Height
            );

            return;
        }

        [metalTexture replaceRegion:MTLRegionMake2D( 0, 0, (NSUInteger)_texture->Width, (NSUInteger)_texture->Height )
                          mipmapLevel:0
                            withBytes:_texture->Pixels
                          bytesPerRow:(NSUInteger)_texture->Width * 4];

        MengineImGUIMetalTexture * backendTexture = [[MengineImGUIMetalTexture alloc] initWithTexture:metalTexture];

        _texture->SetTexID( (ImTextureID)(uintptr_t)(__bridge void *)metalTexture );
        _texture->SetStatus( ImTextureStatus_OK );
        _texture->BackendUserData = (__bridge_retained void *)backendTexture;
    }
    else if( _texture->Status == ImTextureStatus_WantUpdates )
    {
        MengineImGUIMetalTexture * backendTexture = (__bridge MengineImGUIMetalTexture *)_texture->BackendUserData;

        if( backendTexture == nil )
        {
            return;
        }

        for( const ImTextureRect & rect : _texture->Updates )
        {
            [backendTexture.texture replaceRegion:MTLRegionMake2D( (NSUInteger)rect.x, (NSUInteger)rect.y, (NSUInteger)rect.w, (NSUInteger)rect.h )
                                       mipmapLevel:0
                                         withBytes:_texture->GetPixelsAt( rect.x, rect.y )
                                       bytesPerRow:(NSUInteger)_texture->Width * 4];
        }

        _texture->SetStatus( ImTextureStatus_OK );
    }
    else if( _texture->Status == ImTextureStatus_WantDestroy && _texture->UnusedFrames > 0 )
    {
        [self destroyTexture:_texture];
    }
}
//////////////////////////////////////////////////////////////////////////
- (BOOL)createDeviceObjects
{
    if( m_context == nil )
    {
        return NO;
    }

    MTLDepthStencilDescriptor * depthStencilDescriptor = [[MTLDepthStencilDescriptor alloc] init];
    depthStencilDescriptor.depthWriteEnabled = NO;
    depthStencilDescriptor.depthCompareFunction = MTLCompareFunctionAlways;
    m_context.depthStencilState = [m_context.device newDepthStencilStateWithDescriptor:depthStencilDescriptor];

    MTLSamplerDescriptor * samplerDescriptor = [[MTLSamplerDescriptor alloc] init];
    samplerDescriptor.minFilter = MTLSamplerMinMagFilterLinear;
    samplerDescriptor.magFilter = MTLSamplerMinMagFilterLinear;
    samplerDescriptor.mipFilter = MTLSamplerMipFilterLinear;
    m_context.samplerStateLinear = [m_context.device newSamplerStateWithDescriptor:samplerDescriptor];

    samplerDescriptor.minFilter = MTLSamplerMinMagFilterNearest;
    samplerDescriptor.magFilter = MTLSamplerMinMagFilterNearest;
    samplerDescriptor.mipFilter = MTLSamplerMipFilterNearest;
    m_context.samplerStateNearest = [m_context.device newSamplerStateWithDescriptor:samplerDescriptor];

    return YES;
}
//////////////////////////////////////////////////////////////////////////
- (void)destroyDeviceObjects
{
    if( m_context == nil )
    {
        return;
    }

    for( ImTextureData * texture : ImGui::GetPlatformIO().Textures )
    {
        if( texture->RefCount == 1 )
        {
            [self destroyTexture:texture];
        }
    }

    [m_context.renderPipelineStateCache removeAllObjects];
    m_context.depthStencilState = nil;
    m_context.samplerStateLinear = nil;
    m_context.samplerStateNearest = nil;
}
//////////////////////////////////////////////////////////////////////////
- (void)setupRenderState:(ImDrawData *)_drawData pipelineState:(id<MTLRenderPipelineState>)_pipelineState vertexBuffer:(MengineImGUIMetalBuffer *)_vertexBuffer vertexBufferOffset:(size_t)_vertexBufferOffset
{
    if( m_renderCommandEncoder == nil )
    {
        return;
    }

    [m_renderCommandEncoder setCullMode:MTLCullModeNone];
    [m_renderCommandEncoder setDepthStencilState:m_context.depthStencilState];

    MTLViewport viewport =
    {
        .originX = 0.0,
        .originY = 0.0,
        .width = (double)(_drawData->DisplaySize.x * _drawData->FramebufferScale.x),
        .height = (double)(_drawData->DisplaySize.y * _drawData->FramebufferScale.y),
        .znear = 0.0,
        .zfar = 1.0
    };
    [m_renderCommandEncoder setViewport:viewport];

    float L = _drawData->DisplayPos.x;
    float R = _drawData->DisplayPos.x + _drawData->DisplaySize.x;
    float T = _drawData->DisplayPos.y;
    float B = _drawData->DisplayPos.y + _drawData->DisplaySize.y;
    float N = (float)viewport.znear;
    float F = (float)viewport.zfar;

    const float orthoProjection[4][4] =
    {
        {2.f / (R - L), 0.f, 0.f, 0.f},
        {0.f, 2.f / (T - B), 0.f, 0.f},
        {0.f, 0.f, 1.f / (F - N), 0.f},
        {(R + L) / (L - R), (T + B) / (B - T), N / (F - N), 1.f},
    };

    [m_renderCommandEncoder setVertexBytes:&orthoProjection length:sizeof(orthoProjection) atIndex:1];
    [m_renderCommandEncoder setRenderPipelineState:_pipelineState];
    [m_renderCommandEncoder setFragmentSamplerState:m_context.samplerStateLinear atIndex:0];
    [m_renderCommandEncoder setVertexBuffer:_vertexBuffer.buffer offset:_vertexBufferOffset atIndex:0];
}
//////////////////////////////////////////////////////////////////////////
- (void)setFragmentSamplerLinear
{
    if( m_renderCommandEncoder != nil )
    {
        [m_renderCommandEncoder setFragmentSamplerState:m_context.samplerStateLinear atIndex:0];
    }
}
//////////////////////////////////////////////////////////////////////////
- (void)setFragmentSamplerNearest
{
    if( m_renderCommandEncoder != nil )
    {
        [m_renderCommandEncoder setFragmentSamplerState:m_context.samplerStateNearest atIndex:0];
    }
}
//////////////////////////////////////////////////////////////////////////
@end

#endif
