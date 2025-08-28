#include "MetalRenderSystem.h"

#include "MetalRenderTypes.h"
#include "MetalRenderVertexBuffer.h"
#include "MetalRenderIndexBuffer.h"
#include "MetalRenderVertexAttribute.h"
#include "MetalRenderFragmentShader.h"
#include "MetalRenderVertexShader.h"
#include "MetalRenderProgram.h"
#include "MetalRenderProgramVariable.h"
#include "MetalRenderImage.h"
#include "MetalRenderTargetTexture.h"
#include "MetalRenderMaterialStageCache.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/FactorableUnique.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Vector.h"
#include "Interface/MemoryServiceInterface.h"
#include "math/uv4.h"

namespace Mengine
{
    SERVICE_FACTORY( MetalRenderSystem, Mengine::MetalRenderSystem );
    //////////////////////////////////////////////////////////////////////////
    MetalRenderSystem::MetalRenderSystem()
    {
        m_renderSystemName = STRINGIZE_STRING_LOCAL( "Metal" );

        m_projectionMatrix = mt::mat4f::identity();
        m_viewMatrix = mt::mat4f::identity();
        m_worldMatrix = mt::mat4f::identity();

        for( uint32_t i = 0; i != METAL_MAX_TEXTURE_STAGES; ++i )
        {
            m_textureMatrix[i] = mt::mat4f::identity();
        }

        m_scissorEnabled = false;
        m_viewport = Viewport( mt::vec2f::identity(), mt::vec2f::identity() );
        m_scissor = m_viewport;

        m_currentVertexBuffer = nullptr;
        m_currentIndexBuffer = nullptr;
        m_currentProgram = nullptr;
        m_currentProgramVariable = nullptr;

        for( uint32_t i = 0; i != METAL_MAX_TEXTURE_STAGES; ++i )
        {
            m_textures[i] = nullptr;
            TextureStageState & stage = m_textureStages[i];
            stage.addressU = TAM_CLAMP;
            stage.addressV = TAM_CLAMP;
            stage.addressBorder = 0;
            stage.minification = TF_POINT;
            stage.mipmap = TF_NONE;
            stage.magnification = TF_POINT;
        }

        m_textureFactor = 0;

        m_blendSrc = BF_ONE;
        m_blendDst = BF_ZERO;
        m_blendOp = BOP_ADD;
        m_blendSrcAlpha = BF_ONE;
        m_blendDstAlpha = BF_ZERO;
        m_blendOpAlpha = BOP_ADD;
        m_separateBlend = false;
        m_alphaBlend = false;

        m_cullMode = CM_NONE;
        m_depthTest = false;
        m_depthWrite = false;
        m_depthFunction = CF_LESS_EQUAL;
        m_fillMode = FM_SOLID;
        m_colorWriteR = true;
        m_colorWriteG = true;
        m_colorWriteB = true;
        m_colorWriteA = true;
        m_vsync = false;

        m_device = nil;
        m_commandQueue = nil;
        m_metalLayer = nil;
        m_commandBuffer = nil;
    }
    //////////////////////////////////////////////////////////////////////////
    MetalRenderSystem::~MetalRenderSystem() = default;
    //////////////////////////////////////////////////////////////////////////
    bool MetalRenderSystem::_initializeService()
    {
        m_device = MTLCreateSystemDefaultDevice();

        if( m_device == nil )
        {
            LOGGER_ERROR( "MetalRenderSystem: failed to create device" );

            return false;
        }

        m_commandQueue = [m_device newCommandQueue];

        if( m_commandQueue == nil )
        {
            LOGGER_ERROR( "MetalRenderSystem: failed to create command queue" );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderSystem::_finalizeService()
    {
        m_commandBuffer = nil;
        m_metalLayer = nil;
        m_commandQueue = nil;
        m_device = nil;
    }
    //////////////////////////////////////////////////////////////////////////
    ERenderPlatform MetalRenderSystem::getRenderPlatformType() const
    {
        return RP_METAL;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & MetalRenderSystem::getRenderPlatformName() const
    {
        return m_renderSystemName;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetalRenderSystem::createRenderWindow( const RenderWindowDesc * _windowDesc )
    {
        m_metalLayer = [CAMetalLayer layer];
        [m_metalLayer setDevice:m_device];

        if( _windowDesc != nullptr )
        {
            m_metalLayer.drawableSize = CGSizeMake( _windowDesc->resolution.getWidth(), _windowDesc->resolution.getHeight() );
            m_vsync = _windowDesc->waitForVSync;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderSystem::destroyRenderWindow()
    {
        m_metalLayer = nil;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderSystem::setProjectionMatrix( const mt::mat4f & _projection )
    {
        m_projectionMatrix = _projection;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderSystem::setViewMatrix( const mt::mat4f & _view )
    {
        m_viewMatrix = _view;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderSystem::setTextureMatrix( uint32_t _stage, const mt::mat4f & _texture )
    {
        if( _stage < METAL_MAX_TEXTURE_STAGES )
        {
            m_textureMatrix[_stage] = _texture;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderSystem::setWorldMatrix( const mt::mat4f & _world )
    {
        m_worldMatrix = _world;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderVertexBufferInterfacePtr MetalRenderSystem::createVertexBuffer( uint32_t _vertexSize, EBufferType _bufferType, const DocumentInterfacePtr & _doc )
    {
        MetalRenderVertexBufferPtr buffer = Helper::makeFactorableUnique<MetalRenderVertexBuffer>( _doc, m_device );

        MENGINE_ASSERTION_MEMORY_PANIC( buffer, "invalid create vertex buffer" );

        if( buffer->initialize( _vertexSize, _bufferType ) == false )
        {
            LOGGER_ERROR( "MetalRenderSystem: failed to initialize vertex buffer" );

            return nullptr;
        }

        return buffer;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetalRenderSystem::setVertexBuffer( const RenderVertexBufferInterfacePtr & _vertexBuffer )
    {
        m_currentVertexBuffer = _vertexBuffer;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderIndexBufferInterfacePtr MetalRenderSystem::createIndexBuffer( uint32_t _indexSize, EBufferType _bufferType, const DocumentInterfacePtr & _doc )
    {
        MetalRenderIndexBufferPtr buffer = Helper::makeFactorableUnique<MetalRenderIndexBuffer>( _doc, m_device );

        MENGINE_ASSERTION_MEMORY_PANIC( buffer, "invalid create index buffer" );

        if( buffer->initialize( _indexSize, _bufferType ) == false )
        {
            LOGGER_ERROR( "MetalRenderSystem: failed to initialize index buffer" );

            return nullptr;
        }

        return buffer;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetalRenderSystem::setIndexBuffer( const RenderIndexBufferInterfacePtr & _indexBuffer )
    {
        m_currentIndexBuffer = _indexBuffer;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderVertexAttributeInterfacePtr MetalRenderSystem::createVertexAttribute( const ConstString & _name, uint32_t _elementSize, const DocumentInterfacePtr & _doc )
    {
        MetalRenderVertexAttributePtr attribute = Helper::makeFactorableUnique<MetalRenderVertexAttribute>( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( attribute, "invalid create vertex attribute '%s'", _name.c_str() );

        if( attribute->initialize( _name, _elementSize ) == false )
        {
            LOGGER_ERROR( "MetalRenderSystem: failed to initialize vertex attribute '%s'", _name.c_str() );

            return nullptr;
        }

        return attribute;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderFragmentShaderInterfacePtr MetalRenderSystem::createFragmentShader( const ConstString & _name, const MemoryInterfacePtr & _memory, bool _compile, const DocumentInterfacePtr & _doc )
    {
        MetalRenderFragmentShaderPtr shader = Helper::makeFactorableUnique<MetalRenderFragmentShader>( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( shader, "invalid create fragment shader '%s'", _name.c_str() );

        if( shader->initialize( _name, _memory, _compile ) == false )
        {
            LOGGER_ERROR( "MetalRenderSystem: failed to initialize fragment shader '%s'", _name.c_str() );

            return nullptr;
        }

        return shader;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderVertexShaderInterfacePtr MetalRenderSystem::createVertexShader( const ConstString & _name, const MemoryInterfacePtr & _memory, bool _compile, const DocumentInterfacePtr & _doc )
    {
        MetalRenderVertexShaderPtr shader = Helper::makeFactorableUnique<MetalRenderVertexShader>( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( shader, "invalid create vertex shader '%s'", _name.c_str() );

        if( shader->initialize( _name, _memory, _compile ) == false )
        {
            LOGGER_ERROR( "MetalRenderSystem: failed to initialize vertex shader '%s'", _name.c_str() );

            return nullptr;
        }

        return shader;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderProgramInterfacePtr MetalRenderSystem::createProgram( const ConstString & _name, const RenderVertexShaderInterfacePtr & _vertex, const RenderFragmentShaderInterfacePtr & _fragment, const RenderVertexAttributeInterfacePtr & _vertexAttribute, uint32_t _samplerCount, const DocumentInterfacePtr & _doc )
    {
        MetalRenderProgramPtr program = Helper::makeFactorableUnique<MetalRenderProgram>( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( program, "invalid create program '%s'", _name.c_str() );

        if( program->initialize( _name, _vertex, _fragment, _vertexAttribute, _samplerCount ) == false )
        {
            LOGGER_ERROR( "MetalRenderSystem: failed to initialize program '%s'", _name.c_str() );

            return nullptr;
        }

        return program;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderSystem::setProgram( const RenderProgramInterfacePtr & _program )
    {
        m_currentProgram = _program;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderSystem::updateProgram( const RenderProgramInterfacePtr & _program )
    {
        m_currentProgram = _program;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderProgramVariableInterfacePtr MetalRenderSystem::createProgramVariable( uint32_t _vertexCount, uint32_t _pixelCount, const DocumentInterfacePtr & _doc )
    {
        MetalRenderProgramVariablePtr variable = Helper::makeFactorableUnique<MetalRenderProgramVariable>( _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( variable, "invalid create program variable" );

        if( variable->initialize( _vertexCount, _pixelCount ) == false )
        {
            LOGGER_ERROR( "MetalRenderSystem: failed to initialize program variable" );

            return nullptr;
        }

        return variable;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetalRenderSystem::setProgramVariable( const RenderProgramInterfacePtr & _program, const RenderProgramVariableInterfacePtr & _variable )
    {
        if( m_currentProgram != _program )
        {
            m_currentProgram = _program;
        }

        m_currentProgramVariable = _variable;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderSystem::drawIndexedPrimitive( const RenderMaterialStageCacheInterfacePtr & _stageCache, const RenderIndexedPrimitiveDesc & _desc )
    {
        MENGINE_UNUSED( _stageCache );

        if( m_commandBuffer == nil || m_metalLayer == nil )
        {
            return;
        }

        MetalRenderVertexBuffer * vb = m_currentVertexBuffer.getT<MetalRenderVertexBuffer *>();
        MetalRenderIndexBuffer * ib = m_currentIndexBuffer.getT<MetalRenderIndexBuffer *>();

        id<CAMetalDrawable> drawable = [m_metalLayer nextDrawable];

        if( drawable == nil )
        {
            return;
        }

        MTLRenderPassDescriptor * pass = [MTLRenderPassDescriptor renderPassDescriptor];
        pass.colorAttachments[0].texture = drawable.texture;
        pass.colorAttachments[0].loadAction = MTLLoadActionLoad;
        pass.colorAttachments[0].storeAction = MTLStoreActionStore;

        id<MTLRenderCommandEncoder> encoder = [m_commandBuffer renderCommandEncoderWithDescriptor:pass];

        if( vb != nullptr )
        {
            [encoder setVertexBuffer:vb->getBuffer() offset:0 atIndex:0];
        }

        if( ib != nullptr )
        {
            MTLIndexType indexType = (m_currentIndexBuffer->getIndexSize() == 2) ? MTLIndexTypeUInt16 : MTLIndexTypeUInt32;
            [encoder drawIndexedPrimitives:MTLPrimitiveTypeTriangle indexCount:_desc.indexCount indexType:indexType indexBuffer:ib->getBuffer() indexBufferOffset:_desc.startIndex * m_currentIndexBuffer->getIndexSize()];
        }

        [encoder endEncoding];

        [m_commandBuffer presentDrawable:drawable];
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderSystem::setTexture( const RenderProgramInterfacePtr & _program, uint32_t _stage, const RenderImageInterfacePtr & _texture )
    {
        if( m_currentProgram != _program )
        {
            m_currentProgram = _program;
        }

        if( _stage >= METAL_MAX_TEXTURE_STAGES )
        {
            return;
        }

        m_textures[_stage] = _texture;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderSystem::setTextureAddressing( uint32_t _stage, ETextureAddressMode _modeU, ETextureAddressMode _modeV, uint32_t _border )
    {
        if( _stage >= METAL_MAX_TEXTURE_STAGES )
        {
            return;
        }

        m_textureStages[_stage].addressU = _modeU;
        m_textureStages[_stage].addressV = _modeV;
        m_textureStages[_stage].addressBorder = _border;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderSystem::setTextureFactor( uint32_t _color )
    {
        m_textureFactor = _color;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderSystem::setBlendFactor( EBlendFactor _src, EBlendFactor _dst, EBlendOp _op, EBlendFactor _separateSrc, EBlendFactor _separateDst, EBlendOp _separateOp, bool _separate )
    {
        m_blendSrc = _src;
        m_blendDst = _dst;
        m_blendOp = _op;
        m_blendSrcAlpha = _separateSrc;
        m_blendDstAlpha = _separateDst;
        m_blendOpAlpha = _separateOp;
        m_separateBlend = _separate;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderSystem::setCullMode( ECullMode _mode )
    {
        m_cullMode = _mode;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderSystem::setDepthBufferTestEnable( bool _depthTest )
    {
        m_depthTest = _depthTest;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderSystem::setDepthBufferWriteEnable( bool _depthWrite )
    {
        m_depthWrite = _depthWrite;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderSystem::setDepthBufferCmpFunc( ECompareFunction _depthFunction )
    {
        m_depthFunction = _depthFunction;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderSystem::setFillMode( EFillMode _mode )
    {
        m_fillMode = _mode;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderSystem::setColorBufferWriteEnable( bool _r, bool _g, bool _b, bool _a )
    {
        m_colorWriteR = _r;
        m_colorWriteG = _g;
        m_colorWriteB = _b;
        m_colorWriteA = _a;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderSystem::setAlphaBlendEnable( bool _alphaBlend )
    {
        m_alphaBlend = _alphaBlend;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderSystem::setTextureStageFilter( uint32_t _stage, ETextureFilter _minification, ETextureFilter _mipmap, ETextureFilter _magnification )
    {
        if( _stage >= METAL_MAX_TEXTURE_STAGES )
        {
            return;
        }

        m_textureStages[_stage].minification = _minification;
        m_textureStages[_stage].mipmap = _mipmap;
        m_textureStages[_stage].magnification = _magnification;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderImageInterfacePtr MetalRenderSystem::createImage( uint32_t _mipmaps, uint32_t _width, uint32_t _height, EPixelFormat _format, const DocumentInterfacePtr & _doc )
    {
        MetalRenderImagePtr image = Helper::makeFactorableUnique<MetalRenderImage>( _doc, m_device );

        MENGINE_ASSERTION_MEMORY_PANIC( image, "invalid create metal image" );

        if( image->initialize( _mipmaps, _width, _height, _format ) == false )
        {
            return nullptr;
        }

        return image;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderTargetInterfacePtr MetalRenderSystem::createRenderTargetTexture( uint32_t _width, uint32_t _height, EPixelFormat _format, const DocumentInterfacePtr & _doc )
    {
        MetalRenderTargetTexturePtr target = Helper::makeFactorableUnique<MetalRenderTargetTexture>( _doc, m_device );

        MENGINE_ASSERTION_MEMORY_PANIC( target, "invalid create metal render target" );

        if( target->initialize( _width, _height, _format ) == false )
        {
            return nullptr;
        }

        return target;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderTargetInterfacePtr MetalRenderSystem::createRenderTargetOffscreen( uint32_t _width, uint32_t _height, EPixelFormat _format, const DocumentInterfacePtr & _doc )
    {
        return this->createRenderTargetTexture( _width, _height, _format, _doc );
    }
    //////////////////////////////////////////////////////////////////////////
    RenderImageInterfacePtr MetalRenderSystem::createRenderImageTarget( const RenderTargetInterfacePtr & _renderTarget, const DocumentInterfacePtr & _doc )
    {
        MetalRenderTargetTexture * target = _renderTarget.getT<MetalRenderTargetTexture *>();

        MetalRenderImagePtr image = Helper::makeFactorableUnique<MetalRenderImage>( _doc, target->getDevice() );

        if( image->initializeFromTexture( target->getTexture(), target->getHWWidth(), target->getHWHeight(), target->getHWPixelFormat() ) == false )
        {
            return nullptr;
        }

        return image;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderMaterialStageCacheInterfacePtr MetalRenderSystem::createRenderMaterialStageCache( const RenderMaterialStage * _stage, const DocumentInterfacePtr & _doc )
    {
        MENGINE_UNUSED( _stage );

        MetalRenderMaterialStageCachePtr cache = Helper::makeFactorableUnique<MetalRenderMaterialStageCache>( _doc );
        MENGINE_ASSERTION_MEMORY_PANIC( cache, "invalid create material stage cache" );
        return cache;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetalRenderSystem::beginScene()
    {
        m_commandBuffer = [m_commandQueue commandBuffer];

        return m_commandBuffer != nil;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderSystem::endScene()
    {
        if( m_commandBuffer != nil )
        {
            [m_commandBuffer commit];
            m_commandBuffer = nil;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderSystem::swapBuffers()
    {
        // presentation handled during drawIndexedPrimitive
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderSystem::clearFrameBuffer( uint32_t _frameBufferTypes, const Color & _color, double _depth, int32_t _stencil )
    {
        MENGINE_UNUSED( _depth );
        MENGINE_UNUSED( _stencil );

        if( ( _frameBufferTypes & FBT_COLOR ) == 0 )
        {
            return;
        }

        if( m_commandBuffer == nil || m_metalLayer == nil )
        {
            return;
        }

        id<CAMetalDrawable> drawable = [m_metalLayer nextDrawable];

        if( drawable == nil )
        {
            return;
        }

        MTLRenderPassDescriptor * pass = [MTLRenderPassDescriptor renderPassDescriptor];
        pass.colorAttachments[0].texture = drawable.texture;
        pass.colorAttachments[0].loadAction = MTLLoadActionClear;
        pass.colorAttachments[0].storeAction = MTLStoreActionStore;
        pass.colorAttachments[0].clearColor = MTLClearColorMake( _color.getR(), _color.getG(), _color.getB(), _color.getA() );

        id<MTLRenderCommandEncoder> encoder = [m_commandBuffer renderCommandEncoderWithDescriptor:pass];
        [encoder endEncoding];

        [m_commandBuffer presentDrawable:drawable];
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderSystem::setScissor( const Viewport & _viewport )
    {
        m_scissor = _viewport;
        m_scissorEnabled = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderSystem::removeScissor()
    {
        m_scissorEnabled = false;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderSystem::setViewport( const Viewport & _viewport )
    {
        m_viewport = _viewport;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderSystem::changeWindowMode( const Resolution & _resolution, bool _fullscreen )
    {
        MENGINE_UNUSED( _resolution );
        MENGINE_UNUSED( _fullscreen );
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderSystem::onWindowMovedOrResized()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderSystem::onWindowClose()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderSystem::onDeviceLostPrepare()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetalRenderSystem::onDeviceLostRestore()
    {
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderSystem::onWindowChangeFullscreenPrepare( bool _fullscreen )
    {
        MENGINE_UNUSED( _fullscreen );
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetalRenderSystem::onWindowChangeFullscreen( bool _fullscreen )
    {
        MENGINE_UNUSED( _fullscreen );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetalRenderSystem::setVSync( bool _vSync )
    {
        m_vsync = _vSync;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t MetalRenderSystem::getMaxCombinedTextureImageUnits() const
    {
        return METAL_MAX_TEXTURE_STAGES;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t MetalRenderSystem::getAvailableTextureMemory() const
    {
        return 0U;
    }
    //////////////////////////////////////////////////////////////////////////
}

