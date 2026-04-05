#include "MetalRenderProgram.h"

#include "Environment/Metal/MetalRenderSystemExtensionInterface.h"

#include "MetalRenderEnum.h"

#include "Kernel/Logger.h"

#import <Metal/Metal.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MetalRenderProgram::MetalRenderProgram()
        : m_samplerCount( 0 )
        , m_pipelineState( nil )
        , m_deferredCompile( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MetalRenderProgram::~MetalRenderProgram()
    {
        MENGINE_ASSERTION_FATAL( m_pipelineState == nil, "program is not released" );
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & MetalRenderProgram::getName() const
    {
        return m_name;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderVertexAttributeInterfacePtr & MetalRenderProgram::getVertexAttribute() const
    {
        return m_vertexAttribute;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderFragmentShaderInterfacePtr & MetalRenderProgram::getFragmentShader() const
    {
        return m_fragmentShader;
    }
    //////////////////////////////////////////////////////////////////////////
    const RenderVertexShaderInterfacePtr & MetalRenderProgram::getVertexShader() const
    {
        return m_vertexShader;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetalRenderProgram::initialize( const ConstString & _name, const MetalRenderVertexShaderPtr & _vertexShader, const MetalRenderFragmentShaderPtr & _fragmentShader, const MetalRenderVertexAttributePtr & _vertexAttribute, uint32_t _samplerCount )
    {
        m_name = _name;
        m_vertexShader = _vertexShader;
        m_fragmentShader = _fragmentShader;
        m_vertexAttribute = _vertexAttribute;
        m_samplerCount = _samplerCount;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderProgram::finalize()
    {
        m_vertexShader = nullptr;
        m_fragmentShader = nullptr;
        m_vertexAttribute = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderProgram::setDeferredCompile( bool _deferredCompile )
    {
        m_deferredCompile = _deferredCompile;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetalRenderProgram::getDeferredCompile() const
    {
        return m_deferredCompile;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetalRenderProgram::compile()
    {
        MENGINE_ASSERTION_FATAL( m_samplerCount <= MENGINE_MAX_TEXTURE_STAGES, "program '%s' don't support sampler count %d max %d"
            , m_name.c_str()
            , m_samplerCount
            , MENGINE_MAX_TEXTURE_STAGES
        );

        LOGGER_INFO( "render", "compile program '%s'"
            , m_name.c_str()
        );

        if( m_vertexShader != nullptr )
        {
            if( m_vertexShader->compile() == false )
            {
                LOGGER_ERROR( "invalid compile program '%s' vertex shader '%s'"
                    , m_name.c_str()
                    , m_vertexShader->getName().c_str()
                );

                return false;
            }
        }

        if( m_fragmentShader != nullptr )
        {
            if( m_fragmentShader->compile() == false )
            {
                LOGGER_ERROR( "invalid compile program '%s' fragment shader '%s'"
                    , m_name.c_str()
                    , m_fragmentShader->getName().c_str()
                );

                return false;
            }
        }

        MetalRenderSystemExtensionInterface * extension = RENDER_SYSTEM()
            ->getUnknown();

        id<MTLDevice> device = extension->getMetalDevice();

        MTLRenderPipelineDescriptor * pipelineDesc = [[MTLRenderPipelineDescriptor alloc] init];
        pipelineDesc.label = [NSString stringWithUTF8String:m_name.c_str()];

        if( m_vertexShader != nullptr )
        {
            pipelineDesc.vertexFunction = m_vertexShader->getFunction();
        }

        if( m_fragmentShader != nullptr )
        {
            pipelineDesc.fragmentFunction = m_fragmentShader->getFunction();
        }

        pipelineDesc.colorAttachments[0].pixelFormat = MTLPixelFormatBGRA8Unorm;

        pipelineDesc.colorAttachments[0].blendingEnabled = YES;
        pipelineDesc.colorAttachments[0].sourceRGBBlendFactor = MTLBlendFactorSourceAlpha;
        pipelineDesc.colorAttachments[0].destinationRGBBlendFactor = MTLBlendFactorOneMinusSourceAlpha;
        pipelineDesc.colorAttachments[0].rgbBlendOperation = MTLBlendOperationAdd;
        pipelineDesc.colorAttachments[0].sourceAlphaBlendFactor = MTLBlendFactorOne;
        pipelineDesc.colorAttachments[0].destinationAlphaBlendFactor = MTLBlendFactorOneMinusSourceAlpha;
        pipelineDesc.colorAttachments[0].alphaBlendOperation = MTLBlendOperationAdd;

        pipelineDesc.depthAttachmentPixelFormat = MTLPixelFormatDepth32Float_Stencil8;
        pipelineDesc.stencilAttachmentPixelFormat = MTLPixelFormatDepth32Float_Stencil8;

        // Build vertex descriptor from attributes
        if( m_vertexAttribute != nullptr )
        {
            MTLVertexDescriptor * vertexDesc = [[MTLVertexDescriptor alloc] init];

            const MetalRenderVertexAttribute::VectorAttribute & attributes = m_vertexAttribute->getAttributes();

            uint32_t attrIndex = 0;

            for( const MetalRenderVertexAttribute::AttributeDesc & attr : attributes )
            {
                MTLVertexFormat format = Helper::toMTLVertexFormat( attr.type, attr.size );

                vertexDesc.attributes[attrIndex].format = format;
                vertexDesc.attributes[attrIndex].offset = attr.offset;
                vertexDesc.attributes[attrIndex].bufferIndex = 0;

                ++attrIndex;
            }

            if( !attributes.empty() )
            {
                vertexDesc.layouts[0].stride = m_vertexAttribute->getElementSize();
                vertexDesc.layouts[0].stepFunction = MTLVertexStepFunctionPerVertex;
                vertexDesc.layouts[0].stepRate = 1;
            }

            pipelineDesc.vertexDescriptor = vertexDesc;
        }

        NSError * error = nil;
        id<MTLRenderPipelineState> pipelineState = [device newRenderPipelineStateWithDescriptor:pipelineDesc error:&error];

        if( pipelineState == nil )
        {
            LOGGER_ERROR( "program '%s' pipeline state creation error: %s"
                , m_name.c_str()
                , [[error localizedDescription] UTF8String]
            );

            return false;
        }

        m_pipelineState = pipelineState;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderProgram::release()
    {
        LOGGER_INFO( "render", "release program '%s'"
            , m_name.c_str()
        );

        m_pipelineState = nil;

        if( m_vertexShader != nullptr )
        {
            m_vertexShader->release();
        }

        if( m_fragmentShader != nullptr )
        {
            m_fragmentShader->release();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t MetalRenderProgram::getSamplerCount() const
    {
        return m_samplerCount;
    }
    //////////////////////////////////////////////////////////////////////////
    const MetalRenderVertexAttributePtr & MetalRenderProgram::getMetalVertexAttribute() const
    {
        return m_vertexAttribute;
    }
    //////////////////////////////////////////////////////////////////////////
    id<MTLRenderPipelineState> MetalRenderProgram::getPipelineState() const
    {
        return m_pipelineState;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderProgram::bindMatrix( id<MTLRenderCommandEncoder> _encoder, const mt::mat4f & _worldMatrix, const mt::mat4f & _viewMatrix, const mt::mat4f & _projectionMatrix, const mt::mat4f & _totalWVPMatrix ) const
    {
        MENGINE_UNUSED( _worldMatrix );
        MENGINE_UNUSED( _viewMatrix );
        MENGINE_UNUSED( _projectionMatrix );

        [_encoder setVertexBytes:_totalWVPMatrix.buff()
                          length:sizeof( mt::mat4f )
                         atIndex:1];
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderProgram::onRenderReset()
    {
        if( m_deferredCompile == true )
        {
            return;
        }

        this->release();
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetalRenderProgram::onRenderRestore()
    {
        if( m_deferredCompile == true )
        {
            return true;
        }

        bool successful = this->compile();

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
}
