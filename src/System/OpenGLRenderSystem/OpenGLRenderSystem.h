#pragma once

#include "Interface/RenderSystemInterface.h"
#include "Interface/PlatformInterface.h"

#include "OpenGLRenderImage.h"
#include "OpenGLRenderVertexAttribute.h"
#include "OpenGLRenderVertexBuffer.h"
#include "OpenGLRenderIndexBuffer.h"
#include "OpenGLRenderProgram.h"
#include "OpenGLRenderExtension.h"

#include "Core/ServiceBase.h"
#include "Factory/Factory.h"


namespace Mengine
{
    struct TextureStage
    {
        TextureStage()
            : texture( nullptr )
            , minFilter( 0 )
            , magFilter( 0 )
            , wrapS( 0 )
            , wrapT( 0 )
            , border( 0 )
        {}

        OpenGLRenderImage * texture;

        GLenum minFilter;
        GLenum magFilter;
        GLenum wrapS;
        GLenum wrapT;

        uint32_t border;
    };

    class OpenGLRenderSystem
        : public ServiceBase<RenderSystemInterface>
    {
    public:
        OpenGLRenderSystem();
        ~OpenGLRenderSystem() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        ERenderPlatform getRenderPlatformType() const override;
        const ConstString & getRenderPlatformName() const override;

    public:
        bool createRenderWindow( const Resolution & _resolution, uint32_t _bits, bool _fullscreen, bool _waitForVSync, int _FSAAType, int _FSAAQuality, uint32_t _MultiSampleCount ) override;

    public:
        bool screenshot( const RenderImageInterfacePtr & _image, const mt::vec4f & _rect ) override;

        void setViewMatrix( const mt::mat4f & _view ) override;
        void setWorldMatrix( const mt::mat4f & _world ) override;
        void setProjectionMatrix( const mt::mat4f & _projection ) override;

        void setTextureMatrix( uint32_t _stage, const mt::mat4f & _texture ) override;

    public:
        RenderVertexBufferInterfacePtr createVertexBuffer( uint32_t _vertexSize, EBufferType _bufferType ) override;
        bool setVertexBuffer( const RenderVertexBufferInterfacePtr & _vertexBuffer ) override;
        
    public:
        RenderIndexBufferInterfacePtr createIndexBuffer( uint32_t _indexSize, EBufferType _bufferType ) override;
        bool setIndexBuffer( const RenderIndexBufferInterfacePtr & _indexBuffer ) override;

    public:
        void drawIndexedPrimitive( EPrimitiveType _type, uint32_t _baseVertexIndex,
            uint32_t _minIndex, uint32_t _verticesNum, uint32_t _startIndex, uint32_t _indexCount ) override;

    public:
        void setTexture( uint32_t _stage, const RenderImageInterfacePtr & _texture ) override;
        void setTextureAddressing( uint32_t _stage, ETextureAddressMode _modeU, ETextureAddressMode _modeV, uint32_t _border ) override;
        void setTextureFactor( uint32_t _color ) override;
        void setBlendFactor( EBlendFactor _src, EBlendFactor _dst, EBlendOp _op ) override;
        void setCullMode( ECullMode _mode ) override;
        void setDepthBufferTestEnable( bool _depthTest ) override;
        void setDepthBufferWriteEnable( bool _depthWrite ) override;
        void setDepthBufferCmpFunc( ECompareFunction _depthFunction ) override;
        void setFillMode( EFillMode _mode ) override;
        void setColorBufferWriteEnable( bool _r, bool _g, bool _b, bool _a ) override;
        void setAlphaBlendEnable( bool _alphaBlend ) override;

    public:
        void setTextureStageFilter( uint32_t _stage, ETextureFilter _minification, ETextureFilter _mipmap, ETextureFilter _magnification ) override;

    public:
        RenderVertexAttributeInterfacePtr createVertexAttribute( const ConstString & _name, uint32_t _elementSize ) override;
        RenderFragmentShaderInterfacePtr createFragmentShader( const ConstString & _name, const MemoryInterfacePtr & _memory, bool _compile ) override;
        RenderVertexShaderInterfacePtr createVertexShader( const ConstString & _name, const MemoryInterfacePtr & _memory, bool _compile ) override;

        RenderProgramInterfacePtr createProgram( const ConstString & _name, const RenderVertexShaderInterfacePtr & _vertex, const RenderFragmentShaderInterfacePtr & _fragment, const RenderVertexAttributeInterfacePtr & _vertexAttribute, uint32_t _samplerCount ) override;
        void setProgram( const RenderProgramInterfacePtr & _program ) override;
        void updateProgram( const RenderProgramInterfacePtr & _program ) override;

    public:
        RenderImageInterfacePtr createImage( uint32_t _mipmaps, uint32_t _width, uint32_t _height, uint32_t _channels, uint32_t _depth, PixelFormat _format ) override;
        RenderImageInterfacePtr createDynamicImage( uint32_t _width, uint32_t _height, uint32_t _channels, uint32_t _depth, PixelFormat _format ) override;

        RenderTargetInterfacePtr createRenderTargetTexture( uint32_t _width, uint32_t _height, uint32_t _channels, PixelFormat _format ) override;
        RenderTargetInterfacePtr createRenderTargetOffscreen( uint32_t _width, uint32_t _height, uint32_t _channels, PixelFormat _format ) override;

        RenderImageInterfacePtr createRenderTargetImage( const RenderTargetInterfacePtr & _renderTarget ) override;

    public:
        bool beginScene() override;
        void endScene() override;
        void swapBuffers() override;
        void clearFrameBuffer( uint32_t _frameBufferTypes, uint32_t _color, float _depth, uint32_t _stencil ) override;

        void setScissor( const Viewport & _viewport ) override;
        void removeScissor() override;

        void setViewport( const Viewport & _viewport ) override;

        void changeWindowMode( const Resolution & _resolution, bool _fullscreen ) override;

        bool supportTextureFormat( PixelFormat _format ) const override;
        bool supportTextureNonPow2() const override;
        uint32_t getMaxCombinedTextureImageUnits() const override;

        void onWindowMovedOrResized() override;
        void onWindowClose() override;

        void onWindowChangeFullscreenPrepare( bool _fullscreen ) override;
        void onWindowChangeFullscreen( bool _fullscreen ) override;

        void setVSync( bool _vSync ) override;

        void clear( uint8_t _r, uint8_t _g, uint8_t _b, bool _force ) override;
        
        uint32_t getTextureMemoryUse() const override;
        uint32_t getTextureCount() const override;

    protected:
        void findFormatFromChannels_( PixelFormat _format, uint32_t _channels, PixelFormat & _hwFormat, uint32_t & _hwChannels ) const;

    protected:
        void onRenderImageDestroy_( OpenGLRenderImage * _image );
        void onRenderVertexShaderDestroy_( OpenGLRenderVertexShader * _vertexShader );
        void onRenderFragmentShaderDestroy_( OpenGLRenderFragmentShader * _fragmentShader );
        void onRenderProgramDestroy_( OpenGLRenderProgram * _program );

    private:
        ConstString m_renderPlatform;

        Viewport m_viewport;

        mt::mat4f m_worldMatrix;
        mt::mat4f m_viewMatrix;
        mt::mat4f m_projectionMatrix;

        Resolution m_resolution;

        RenderVertexBufferInterfacePtr m_currentVertexBuffer;
        RenderIndexBufferInterfacePtr m_currentIndexBuffer;        

        typedef Vector<OpenGLRenderVertexShaderPtr> TVectorRenderVertexShaders;
        TVectorRenderVertexShaders m_deferredCompileVertexShaders;

        typedef Vector<OpenGLRenderFragmentShaderPtr> TVectorRenderFragmentShaders;
        TVectorRenderFragmentShaders m_deferredCompileFragmentShaders;

        typedef Vector<OpenGLRenderProgramPtr> TVectorDeferredRenderPrograms;
        TVectorDeferredRenderPrograms m_deferredCompilePrograms;

        OpenGLRenderProgramPtr m_currentProgram;

        uint32_t m_glMaxClipPlanes;
        uint32_t m_glMaxCombinedTextureImageUnits;

        TextureStage m_textureStage[MENGINE_MAX_TEXTURE_STAGES];

        typedef Vector<OpenGLRenderImage *> TVectorCacheRenderImages;
        TVectorCacheRenderImages m_cacheRenderImages;

        typedef Vector<OpenGLRenderVertexShader *> TVectorCacheRenderVertexShaders;
        TVectorCacheRenderVertexShaders m_cacheRenderVertexShaders;

        typedef Vector<OpenGLRenderFragmentShader *> TVectorCacheRenderFragmentShaders;
        TVectorCacheRenderFragmentShaders m_cacheRenderFragmentShaders;

        typedef Vector<OpenGLRenderProgram *> TVectorCacheRenderPrograms;
        TVectorCacheRenderPrograms m_cacheRenderPrograms;

        bool m_renderWindowCreate;
        bool m_depthMask;

        FactoryPtr m_factoryVertexBuffer;
        FactoryPtr m_factoryIndexBuffer;
        FactoryPtr m_factoryRenderImage;
        FactoryPtr m_factoryRenderVertexAttribute;
        FactoryPtr m_factoryRenderFragmentShader;
        FactoryPtr m_factoryRenderVertexShader;
        FactoryPtr m_factoryProgram;
    };
}
