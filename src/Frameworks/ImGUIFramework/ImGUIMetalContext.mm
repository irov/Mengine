#include "ImGUIMetalContext.h"

#include "Kernel/Logger.h"

#include "imgui.h"

#import <time.h>

//////////////////////////////////////////////////////////////////////////
static double MengineImGUIMetalContextCurrentTime()
{
    return (double)clock_gettime_nsec_np( CLOCK_UPTIME_RAW ) / 1e9;
}
//////////////////////////////////////////////////////////////////////////
@implementation MengineImGUIMetalContext

- (instancetype)init
{
    self = [super init];

    if( self == nil )
    {
        return nil;
    }

    self.renderPipelineStateCache = [NSMutableDictionary dictionary];
    self.bufferCache = [NSMutableArray array];
    self.bufferCacheLock = [[NSObject alloc] init];
    self.lastBufferCachePurge = MengineImGUIMetalContextCurrentTime();

    return self;
}

- (MengineImGUIMetalBuffer *)dequeueReusableBufferOfLength:(NSUInteger)_length device:(id<MTLDevice>)_device
{
    double now = MengineImGUIMetalContextCurrentTime();

    @synchronized( self.bufferCacheLock )
    {
        if( now - self.lastBufferCachePurge > 1.0 )
        {
            NSMutableArray<MengineImGUIMetalBuffer *> * survivors = [NSMutableArray array];

            for( MengineImGUIMetalBuffer * candidate in self.bufferCache )
            {
                if( candidate.lastReuseTime > self.lastBufferCachePurge )
                {
                    [survivors addObject:candidate];
                }
            }

            self.bufferCache = survivors;
            self.lastBufferCachePurge = now;
        }

        MengineImGUIMetalBuffer * bestCandidate = nil;

        for( MengineImGUIMetalBuffer * candidate in self.bufferCache )
        {
            if( candidate.buffer.length >= _length && (bestCandidate == nil || bestCandidate.lastReuseTime > candidate.lastReuseTime) )
            {
                bestCandidate = candidate;
            }
        }

        if( bestCandidate != nil )
        {
            [self.bufferCache removeObject:bestCandidate];
            bestCandidate.lastReuseTime = now;

            return bestCandidate;
        }
    }

    id<MTLBuffer> buffer = [_device newBufferWithLength:_length options:MTLResourceStorageModeShared];

    return [[MengineImGUIMetalBuffer alloc] initWithBuffer:buffer];
}

- (id<MTLRenderPipelineState>)renderPipelineStateForFramebufferDescriptor:(MengineImGUIFramebufferDescriptor *)_descriptor device:(id<MTLDevice>)_device
{
    NSError * error = nil;

    NSString * shaderSource = @""
        "#include <metal_stdlib>\n"
        "using namespace metal;\n"
        "\n"
        "struct Uniforms {\n"
        "    float4x4 projectionMatrix;\n"
        "};\n"
        "\n"
        "struct VertexIn {\n"
        "    float2 position [[attribute(0)]];\n"
        "    float2 texCoords [[attribute(1)]];\n"
        "    uchar4 color [[attribute(2)]];\n"
        "};\n"
        "\n"
        "struct VertexOut {\n"
        "    float4 position [[position]];\n"
        "    float2 texCoords;\n"
        "    float4 color;\n"
        "};\n"
        "\n"
        "vertex VertexOut vertex_main(VertexIn in [[stage_in]], constant Uniforms & uniforms [[buffer(1)]]) {\n"
        "    VertexOut out;\n"
        "    out.position = uniforms.projectionMatrix * float4(in.position, 0, 1);\n"
        "    out.texCoords = in.texCoords;\n"
        "    out.color = float4(in.color) / float4(255.0);\n"
        "    return out;\n"
        "}\n"
        "\n"
        "fragment half4 fragment_main(VertexOut in [[stage_in]], texture2d<half, access::sample> texture [[texture(0)]], sampler textureSampler [[sampler(0)]]) {\n"
        "    half4 textureColor = texture.sample(textureSampler, in.texCoords);\n"
        "    return half4(in.color) * textureColor;\n"
        "}\n";

    id<MTLLibrary> library = [_device newLibraryWithSource:shaderSource options:nil error:&error];

    if( library == nil )
    {
        LOGGER_ERROR( "invalid create ImGUI Metal library: %s"
            , [[error localizedDescription] UTF8String]
        );

        return nil;
    }

    id<MTLFunction> vertexFunction = [library newFunctionWithName:@"vertex_main"];
    id<MTLFunction> fragmentFunction = [library newFunctionWithName:@"fragment_main"];

    if( vertexFunction == nil || fragmentFunction == nil )
    {
        LOGGER_ERROR( "invalid find ImGUI Metal shader functions" );

        return nil;
    }

    MTLVertexDescriptor * vertexDescriptor = [MTLVertexDescriptor vertexDescriptor];
    vertexDescriptor.attributes[0].offset = offsetof( ImDrawVert, pos );
    vertexDescriptor.attributes[0].format = MTLVertexFormatFloat2;
    vertexDescriptor.attributes[0].bufferIndex = 0;
    vertexDescriptor.attributes[1].offset = offsetof( ImDrawVert, uv );
    vertexDescriptor.attributes[1].format = MTLVertexFormatFloat2;
    vertexDescriptor.attributes[1].bufferIndex = 0;
    vertexDescriptor.attributes[2].offset = offsetof( ImDrawVert, col );
    vertexDescriptor.attributes[2].format = MTLVertexFormatUChar4;
    vertexDescriptor.attributes[2].bufferIndex = 0;
    vertexDescriptor.layouts[0].stepRate = 1;
    vertexDescriptor.layouts[0].stepFunction = MTLVertexStepFunctionPerVertex;
    vertexDescriptor.layouts[0].stride = sizeof( ImDrawVert );

    MTLRenderPipelineDescriptor * pipelineDescriptor = [[MTLRenderPipelineDescriptor alloc] init];
    pipelineDescriptor.vertexFunction = vertexFunction;
    pipelineDescriptor.fragmentFunction = fragmentFunction;
    pipelineDescriptor.vertexDescriptor = vertexDescriptor;
    pipelineDescriptor.rasterSampleCount = _descriptor.sampleCount;
    pipelineDescriptor.colorAttachments[0].pixelFormat = _descriptor.colorPixelFormat;
    pipelineDescriptor.colorAttachments[0].blendingEnabled = YES;
    pipelineDescriptor.colorAttachments[0].rgbBlendOperation = MTLBlendOperationAdd;
    pipelineDescriptor.colorAttachments[0].sourceRGBBlendFactor = MTLBlendFactorSourceAlpha;
    pipelineDescriptor.colorAttachments[0].destinationRGBBlendFactor = MTLBlendFactorOneMinusSourceAlpha;
    pipelineDescriptor.colorAttachments[0].alphaBlendOperation = MTLBlendOperationAdd;
    pipelineDescriptor.colorAttachments[0].sourceAlphaBlendFactor = MTLBlendFactorOne;
    pipelineDescriptor.colorAttachments[0].destinationAlphaBlendFactor = MTLBlendFactorOneMinusSourceAlpha;
    pipelineDescriptor.depthAttachmentPixelFormat = _descriptor.depthPixelFormat;
    pipelineDescriptor.stencilAttachmentPixelFormat = _descriptor.stencilPixelFormat;

    id<MTLRenderPipelineState> pipelineState = [_device newRenderPipelineStateWithDescriptor:pipelineDescriptor error:&error];

    if( pipelineState == nil )
    {
        LOGGER_ERROR( "invalid create ImGUI Metal pipeline state: %s"
            , [[error localizedDescription] UTF8String]
        );
    }

    return pipelineState;
}

@end
