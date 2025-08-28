#pragma once

#include "Interface/RenderSystemInterface.h"
#include "Kernel/ServiceBase.h"

#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>

namespace Mengine
{
    class MetalRenderSystem
        : public ServiceBase<RenderSystemInterface>
    {
        DECLARE_UNKNOWABLE();

    public:
        MetalRenderSystem();
        ~MetalRenderSystem() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        ERenderPlatform getRenderPlatformType() const override;
        const ConstString & getRenderPlatformName() const override;

    public:
        bool createRenderWindow( const RenderWindowDesc * _windowDesc ) override;
        void destroyRenderWindow() override;

    public:
        void setProjectionMatrix( const mt::mat4f & _projection ) override;
        void setViewMatrix( const mt::mat4f & _view ) override;
        void setTextureMatrix( uint32_t _stage, const mt::mat4f & _texture ) override;
        void setWorldMatrix( const mt::mat4f & _world ) override;

    public:
        RenderVertexBufferInterfacePtr createVertexBuffer( uint32_t _vertexSize, EBufferType _bufferType, const DocumentInterfacePtr & _doc ) override;
        bool setVertexBuffer( const RenderVertexBufferInterfacePtr & _vertexBuffer ) override;

        RenderIndexBufferInterfacePtr createIndexBuffer( uint32_t _indexSize, EBufferType _bufferType, const DocumentInterfacePtr & _doc ) override;
        bool setIndexBuffer( const RenderIndexBufferInterfacePtr & _indexBuffer ) override;

    public:
        RenderVertexAttributeInterfacePtr createVertexAttribute( const ConstString & _name, uint32_t _elementSize, const DocumentInterfacePtr & _doc ) override;
        RenderFragmentShaderInterfacePtr createFragmentShader( const ConstString & _name, const MemoryInterfacePtr & _memory, bool _compile, const DocumentInterfacePtr & _doc ) override;
        RenderVertexShaderInterfacePtr createVertexShader( const ConstString & _name, const MemoryInterfacePtr & _memory, bool _compile, const DocumentInterfacePtr & _doc ) override;

        RenderProgramInterfacePtr createProgram( const ConstString & _name, const RenderVertexShaderInterfacePtr & _vertex, const RenderFragmentShaderInterfacePtr & _fragment, const RenderVertexAttributeInterfacePtr & _vertexAttribute, uint32_t _samplerCount, const DocumentInterfacePtr & _doc ) override;
        void setProgram( const RenderProgramInterfacePtr & _program ) override;
        void updateProgram( const RenderProgramInterfacePtr & _program ) override;
        RenderProgramVariableInterfacePtr createProgramVariable( uint32_t _vertexCount, uint32_t _pixelCount, const DocumentInterfacePtr & _doc ) override;
        bool setProgramVariable( const RenderProgramInterfacePtr & _program, const RenderProgramVariableInterfacePtr & _variable ) override;

    public:
        void drawIndexedPrimitive( const RenderMaterialStageCacheInterfacePtr & _stageCache, const RenderIndexedPrimitiveDesc & _desc ) override;

    public:
        void setTexture( const RenderProgramInterfacePtr & _program, uint32_t _stage, const RenderImageInterfacePtr & _texture ) override;
        void setTextureAddressing( uint32_t _stage, ETextureAddressMode _modeU, ETextureAddressMode _modeV, uint32_t _border ) override;
        void setTextureFactor( uint32_t _color ) override;
        void setBlendFactor( EBlendFactor _src, EBlendFactor _dst, EBlendOp _op, EBlendFactor _separateSrc, EBlendFactor _separateDst, EBlendOp _separateOp, bool _separate ) override;
        void setCullMode( ECullMode _mode ) override;
        void setDepthBufferTestEnable( bool _depthTest ) override;
        void setDepthBufferWriteEnable( bool _depthWrite ) override;
        void setDepthBufferCmpFunc( ECompareFunction _depthFunction ) override;
        void setFillMode( EFillMode _mode ) override;
        void setColorBufferWriteEnable( bool _r, bool _g, bool _b, bool _a ) override;
        void setAlphaBlendEnable( bool _alphaBlend ) override;

        void setTextureStageFilter( uint32_t _stage, ETextureFilter _minification, ETextureFilter _mipmap, ETextureFilter _magnification ) override;

    public:
        RenderImageInterfacePtr createImage( uint32_t _mipmaps, uint32_t _width, uint32_t _height, EPixelFormat _format, const DocumentInterfacePtr & _doc ) override;

        RenderTargetInterfacePtr createRenderTargetTexture( uint32_t _width, uint32_t _height, EPixelFormat _format, const DocumentInterfacePtr & _doc ) override;
        RenderTargetInterfacePtr createRenderTargetOffscreen( uint32_t _width, uint32_t _height, EPixelFormat _format, const DocumentInterfacePtr & _doc ) override;

        RenderImageInterfacePtr createRenderImageTarget( const RenderTargetInterfacePtr & _renderTarget, const DocumentInterfacePtr & _doc ) override;

    public:
        RenderMaterialStageCacheInterfacePtr createRenderMaterialStageCache( const RenderMaterialStage * _stage, const DocumentInterfacePtr & _doc ) override;

    public:
        bool beginScene() override;
        void endScene() override;
        void swapBuffers() override;
        void clearFrameBuffer( uint32_t _frameBufferTypes, const Color & _color, double _depth, int32_t _stencil ) override;

        void setScissor( const Viewport & _viewport ) override;
        void removeScissor() override;

        void setViewport( const Viewport & _viewport ) override;

        void changeWindowMode( const Resolution & _resolution, bool _fullscreen ) override;

        void onWindowMovedOrResized() override;
        void onWindowClose() override;

        void onDeviceLostPrepare() override;
        bool onDeviceLostRestore() override;

        void onWindowChangeFullscreenPrepare( bool _fullscreen ) override;
        bool onWindowChangeFullscreen( bool _fullscreen ) override;

        void setVSync( bool _vSync ) override;

    public:
        uint32_t getMaxCombinedTextureImageUnits() const override;

    public:
        uint32_t getAvailableTextureMemory() const override;

    protected:
        ConstString m_renderSystemName;

        static constexpr uint32_t METAL_MAX_TEXTURE_STAGES = 4;

        mt::mat4f m_projectionMatrix;
        mt::mat4f m_viewMatrix;
        mt::mat4f m_worldMatrix;
        mt::mat4f m_textureMatrix[METAL_MAX_TEXTURE_STAGES];

        Viewport m_scissor;
        bool m_scissorEnabled;
        Viewport m_viewport;

        RenderVertexBufferInterfacePtr m_currentVertexBuffer;
        RenderIndexBufferInterfacePtr m_currentIndexBuffer;
        RenderProgramInterfacePtr m_currentProgram;
        RenderProgramVariableInterfacePtr m_currentProgramVariable;

        RenderImageInterfacePtr m_textures[METAL_MAX_TEXTURE_STAGES];

        struct TextureStageState
        {
            ETextureAddressMode addressU;
            ETextureAddressMode addressV;
            uint32_t addressBorder;
            ETextureFilter minification;
            ETextureFilter mipmap;
            ETextureFilter magnification;
        };

        TextureStageState m_textureStages[METAL_MAX_TEXTURE_STAGES];

        uint32_t m_textureFactor;

        EBlendFactor m_blendSrc;
        EBlendFactor m_blendDst;
        EBlendOp m_blendOp;
        EBlendFactor m_blendSrcAlpha;
        EBlendFactor m_blendDstAlpha;
        EBlendOp m_blendOpAlpha;
        bool m_separateBlend;
        bool m_alphaBlend;

        ECullMode m_cullMode;
        bool m_depthTest;
        bool m_depthWrite;
        ECompareFunction m_depthFunction;
        EFillMode m_fillMode;
        bool m_colorWriteR;
        bool m_colorWriteG;
        bool m_colorWriteB;
        bool m_colorWriteA;
        bool m_vsync;

        id<MTLDevice> m_device;
        id<MTLCommandQueue> m_commandQueue;
        CAMetalLayer * m_metalLayer;
        id<MTLCommandBuffer> m_commandBuffer;
    };
}

