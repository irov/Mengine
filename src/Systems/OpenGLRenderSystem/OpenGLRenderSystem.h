#pragma once

#include "Interface/RenderSystemInterface.h"
#include "Interface/FactoryInterface.h"

#include "Environment/OpenGL/OpenGLRenderSystemExtensionInterface.h"

#include "OpenGLRenderImage.h"
#include "OpenGLRenderImageTarget.h"
#include "OpenGLRenderTargetTexture.h"
#include "OpenGLRenderVertexAttribute.h"
#include "OpenGLRenderVertexBuffer.h"
#include "OpenGLRenderIndexBuffer.h"
#include "OpenGLRenderProgram.h"
#include "OpenGLRenderProgramVariable.h"
#include "OpenGLRenderExtension.h"

#include "Kernel/IntrusiveList.h"
#include "Kernel/ServiceBase.h"
#include "Kernel/Hashtable.h"

namespace Mengine
{
    class OpenGLRenderSystem
        : public ServiceBase<RenderSystemInterface>
        , public OpenGLRenderSystemExtensionInterface
    {
        DECLARE_FACTORABLE( OpenGLRenderSystem );
        DECLARE_UNKNOWABLE();

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
        bool createRenderWindow( const RenderWindowDesc * _windowDesc ) override;
        void destroyRenderWindow() override;

    public:
        void setViewMatrix( const mt::mat4f & _view ) override;
        void setWorldMatrix( const mt::mat4f & _world ) override;
        void setProjectionMatrix( const mt::mat4f & _projection ) override;

    public:
        void setTextureMatrix( uint32_t _stage, const mt::mat4f & _texture ) override;

    public:
        RenderVertexBufferInterfacePtr createVertexBuffer( uint32_t _vertexSize, EBufferType _bufferType, const DocumentInterfacePtr & _doc ) override;
        bool setVertexBuffer( const RenderVertexBufferInterfacePtr & _vertexBuffer ) override;

    public:
        RenderIndexBufferInterfacePtr createIndexBuffer( uint32_t _indexSize, EBufferType _bufferType, const DocumentInterfacePtr & _doc ) override;
        bool setIndexBuffer( const RenderIndexBufferInterfacePtr & _indexBuffer ) override;

    public:
        void drawIndexedPrimitive( const RenderMaterialStageCacheInterfacePtr & _stageCache, const RenderIndexedPrimitiveDesc & _desc ) override;

    public:
        void setTexture( const RenderProgramInterfacePtr & _program, uint32_t _stageId, const RenderImageInterfacePtr & _texture ) override;
        void setTextureAddressing( uint32_t _stageId, ETextureAddressMode _modeU, ETextureAddressMode _modeV, uint32_t _border ) override;
        void setTextureFactor( uint32_t _color ) override;
        void setBlendFactor( EBlendFactor _src, EBlendFactor _dst, EBlendOp _op, EBlendFactor _separateSrc, EBlendFactor _separateDst, EBlendOp _separateOp, bool _separate ) override;
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
        RenderVertexAttributeInterfacePtr createVertexAttribute( const ConstString & _name, uint32_t _elementSize, const DocumentInterfacePtr & _doc ) override;
        RenderFragmentShaderInterfacePtr createFragmentShader( const ConstString & _name, const MemoryInterfacePtr & _memory, bool _compile, const DocumentInterfacePtr & _doc ) override;
        RenderVertexShaderInterfacePtr createVertexShader( const ConstString & _name, const MemoryInterfacePtr & _memory, bool _compile, const DocumentInterfacePtr & _doc ) override;

        RenderProgramInterfacePtr createProgram( const ConstString & _name, const RenderVertexShaderInterfacePtr & _vertex, const RenderFragmentShaderInterfacePtr & _fragment, const RenderVertexAttributeInterfacePtr & _vertexAttribute, uint32_t _samplerCount, const DocumentInterfacePtr & _doc ) override;
        void setProgram( const RenderProgramInterfacePtr & _program ) override;
        void updateProgram( const RenderProgramInterfacePtr & _program ) override;
        RenderProgramVariableInterfacePtr createProgramVariable( uint32_t _vertexCount, uint32_t _pixelCount, const DocumentInterfacePtr & _doc ) override;
        bool setProgramVariable( const RenderProgramInterfacePtr & _program, const RenderProgramVariableInterfacePtr & _programVariable ) override;

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

        uint32_t getMaxCombinedTextureImageUnits() const override;

        uint32_t getMaxTextureSize() const override;

        void onWindowMovedOrResized() override;
        void onWindowClose() override;

        void onDeviceLostPrepare() override;
        bool onDeviceLostRestore() override;

        void onWindowChangeFullscreenPrepare( bool _fullscreen ) override;
        bool onWindowChangeFullscreen( bool _fullscreen ) override;

        void setVSync( bool _vSync ) override;

        uint32_t getAvailableTextureMemory() const override;

    protected:
        void findFormatFromChannels_( EPixelFormat _format, EPixelFormat * const _hwFormat ) const;

    protected:
        void onRenderVertexBufferDestroy_( OpenGLRenderVertexBuffer * _vertexBuffer );
        void onRenderIndexBufferDestroy_( OpenGLRenderIndexBuffer * _indexBuffer );
        void onRenderImageDestroy_( OpenGLRenderImage * _image );
        void onRenderImageTargetDestroy_( OpenGLRenderImageTarget * _imageTarget );
        void onRenderTargetTextureDestroy_( OpenGLRenderTargetTexture * _targetTexture );
        void onRenderVertexShaderDestroy_( OpenGLRenderVertexShader * _vertexShader );
        void onRenderFragmentShaderDestroy_( OpenGLRenderFragmentShader * _fragmentShader );
        void onRenderProgramDestroy_( OpenGLRenderProgram * _program );

    protected:
        void updatePMWMatrix_();

    protected:
        GLuint genTexture() override;
        void deleteTexture( GLuint _id ) override;

        GLuint genFramebuffer() override;
        void deleteFramebuffer( GLuint _id ) override;

        GLuint genBuffer() override;
        void deleteBuffer( GLuint _id ) override;

        GLuint genFragmentShader() override;
        void deleteFragmentShader( GLuint _id ) override;

        GLuint genVertexShader() override;
        void deleteVertexShader( GLuint _id ) override;

    protected:
        ConstString m_renderPlatform;

        Viewport m_viewport;

        mt::mat4f m_worldMatrix;
        mt::mat4f m_viewMatrix;
        mt::mat4f m_projectionMatrix;
        mt::mat4f m_totalWVPMatrix;

        Resolution m_windowResolution;
        Viewport m_windowViewport;

        OpenGLRenderProgramPtr m_currentProgram;
        OpenGLRenderProgramVariablePtr m_currentProgramVariable;
        OpenGLRenderVertexAttributePtr m_currentVertexAttribute;
        OpenGLRenderIndexBufferPtr m_currentIndexBuffer;
        OpenGLRenderVertexBufferPtr m_currentVertexBuffer;

        typedef Vector<OpenGLRenderProgramPtr> VectorDeferredRenderPrograms;
        VectorDeferredRenderPrograms m_deferredCompilePrograms;

        uint32_t m_glMaxCombinedTextureImageUnits;
        uint32_t m_glMaxTextureSize;

        struct TextureStage
        {
            RenderImageInterface * texture = nullptr;

            GLenum minFilter = 0;
            GLenum magFilter = 0;
            GLenum wrapS = 0;
            GLenum wrapT = 0;

            uint32_t border = 0;
        };

        TextureStage m_textureStage[MENGINE_MAX_TEXTURE_STAGES];

        typedef IntrusiveList<OpenGLRenderResourceHandler> IntrusiveListOpenGLRenderResourceHandler;
        IntrusiveListOpenGLRenderResourceHandler m_renderResourceHandlers;

        bool m_renderWindowCreate;
        bool m_renderDeviceLost;
        bool m_depthMask;

        Color m_clearColor;

#if defined(MENGINE_RENDER_OPENGL_NORMAL)
        GLclampd m_clearDepth;
#endif

        GLint m_clearStencil;

        FactoryInterfacePtr m_factoryRenderVertexBuffer;
        FactoryInterfacePtr m_factoryRenderIndexBuffer;
        FactoryInterfacePtr m_factoryRenderImage;
        FactoryInterfacePtr m_factoryRenderImageTarget;
        FactoryInterfacePtr m_factoryRenderTargetTexture;
        FactoryInterfacePtr m_factoryRenderVertexAttribute;
        FactoryInterfacePtr m_factoryRenderFragmentShader;
        FactoryInterfacePtr m_factoryRenderVertexShader;
        FactoryInterfacePtr m_factoryRenderProgram;
        FactoryInterfacePtr m_factoryRenderProgramVariable;

#if defined(MENGINE_RENDER_OPENGL_NORMAL)
        GLuint m_vertexArrayId;
#endif
    };
}
