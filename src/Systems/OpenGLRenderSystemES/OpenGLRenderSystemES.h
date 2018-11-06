#	pragma once

#	include "Interface/RenderSystemInterface.h"

#   include "Config/String.h"

#   include "OpenGLRenderImageES.h"
#   include "OpenGLRenderVertexBufferES.h"
#   include "OpenGLRenderIndexBufferES.h"
#   include "OpenGLRenderShaderES.h"
#   include "OpenGLRenderProgramES.h"

#   include "Factory/Factory.h"
#   include "Core/ServiceBase.h"

#   include "stdex/stl_map.h"

#	include "OpenGLRenderHeaderES.h"

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
            , border( 0x00000000 )
            , colorOp( 0 )
            , colorArg1( 0 )
            , colorArg2( 0 )
            , alphaOp( 0 )
            , alphaArg1( 0 )
            , alphaArg2( 0 )
        {
        }

        OpenGLRenderImageES * texture;

        GLenum minFilter;
        GLenum magFilter;
        GLenum wrapS;
        GLenum wrapT;

        uint32_t border;

        GLenum colorOp;
        GLenum colorArg1;
        GLenum colorArg2;
        GLenum alphaOp;
        GLenum alphaArg1;
        GLenum alphaArg2;
    };

    class OpenGLRenderSystemES
        : public ServiceBase<RenderSystemInterface>
    {
    public:
        OpenGLRenderSystemES();
        ~OpenGLRenderSystemES();

    public:
        bool _initialize() override;
        void _finalize() override;

    public:
        const ConstString & getRenderPlatformName() const override;

    public:
        bool createRenderWindow( const Resolution & _resolution, uint32_t _bits, bool _fullscreen, bool _waitForVSync, int _FSAAType, int _FSAAQuality, uint32_t _MultiSampleCount ) override;

        void makeProjectionOrthogonal( mt::mat4f & _projectionMatrix, const Viewport & _viewport, float _near, float _far ) override;
        void makeProjectionFrustum( mt::mat4f & _projectionMatrix, const Viewport & _viewport, float _near, float _far ) override;
        void makeProjectionPerspective( mt::mat4f & _projectionMatrix, float _fov, float _aspect, float zn, float zf ) override;
        void makeViewMatrixFromViewport( mt::mat4f & _viewMatrix, const Viewport & _viewport ) override;
        void makeViewMatrixLookAt( mt::mat4f & _viewMatrix, const mt::vec3f & _eye, const mt::vec3f & _dir, const mt::vec3f & _up, float _sign ) override;

        bool screenshot( const RenderImageInterfacePtr & _image, const mt::vec4f & _rect ) override;

        void setProjectionMatrix( const mt::mat4f & _projection ) override;
        void setModelViewMatrix( const mt::mat4f & _view ) override;
        void setWorldMatrix( const mt::mat4f & _view ) override;

        void setTextureMatrix( uint32_t _stage, const mt::mat4f & _texture ) override;

    public:
        RenderVertexBufferInterfacePtr createVertexBuffer( uint32_t _verticesNum, bool _dynamic ) override;
        bool setVertexBuffer( const RenderVertexBufferInterfacePtr & _vertexBuffer ) override;

        RenderIndexBufferInterfacePtr createIndexBuffer( uint32_t _indiciesNum, bool _dynamic ) override;
        bool setIndexBuffer( const RenderIndexBufferInterfacePtr & _indexBuffer ) override;

    public:
        void drawIndexedPrimitive( EPrimitiveType _type, uint32_t _baseVertexIndex,
            uint32_t _minIndex, uint32_t _verticesNum, uint32_t _startIndex, uint32_t _indexCount ) override;

    protected:
        void bindBuffer_( GLuint _vertexId, GLuint _indexId );

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
        void setShadeType( EShadeType _sType ) override;
        void setAlphaBlendEnable( bool _alphaBlend ) override;
        void setLightingEnable( bool _light ) override;
        void setTextureStageColorOp( uint32_t _stage, ETextureOp _textrueOp, ETextureArgument _arg1, ETextureArgument _arg2 ) override;
        void setTextureStageAlphaOp( uint32_t _stage, ETextureOp _textrueOp, ETextureArgument _arg1, ETextureArgument _arg2 ) override;
        void setTextureStageTexCoordIndex( uint32_t _stage, uint32_t _index ) override;
        void setTextureStageFilter( uint32_t _stage, ETextureFilter _minification, ETextureFilter _mipmap, ETextureFilter _magnification ) override;

    public:
        RenderFragmentShaderInterfacePtr createFragmentShader( const ConstString & _name, const MemoryInterfacePtr & _memory ) override;
        RenderVertexShaderInterfacePtr createVertexShader( const ConstString & _name, const MemoryInterfacePtr & _memory ) override;

        RenderProgramInterfacePtr createProgram( const ConstString & _name, const RenderVertexShaderInterfacePtr & _vertex, const RenderFragmentShaderInterfacePtr & _fragment, uint32_t _samplerCount ) override;
        void setProgram( const RenderProgramInterfacePtr & _program ) override;
        void updateProgram( const RenderProgramInterfacePtr & _program ) override;

    public:
        RenderImageInterfacePtr createImage( uint32_t _mipmaps, uint32_t _width, uint32_t _height, uint32_t _channels, uint32_t _depth, PixelFormat _format ) override;
        RenderImageInterfacePtr createDynamicImage( uint32_t _width, uint32_t _height, uint32_t _channels, uint32_t _depth, PixelFormat _format ) override;

        RenderTargetInterfacePtr createRenderTargetTexture( uint32_t _width, uint32_t _height, PixelFormat _format ) override;
        RenderTargetInterfacePtr createRenderTargetOffscreen( uint32_t _width, uint32_t _height, PixelFormat _format ) override;

        RenderImageInterfacePtr createRenderTargetImage( const RenderTargetInterfacePtr & _renderTarget ) override;

        bool lockRenderTarget( const RenderImageInterfacePtr & _renderTarget ) override;
        bool unlockRenderTarget() override;

        bool beginScene() override;
        void endScene() override;
        void swapBuffers() override;
        void clearFrameBuffer( uint32_t _frameBufferTypes, uint32_t _color, float _depth, uint32_t _stencil ) override;

        void setClipplaneCount( uint32_t _count ) override;
        void setClipplane( uint32_t _i, const mt::planef & _plane ) override;

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

        void clear( uint32_t _color, bool _force ) override;

        void setSeparateAlphaBlendMode() override;

        size_t getTextureMemoryUse() const override;
        uint32_t getTextureCount() const override;

    protected:
        void findFormatFromChannels_( PixelFormat _format, uint32_t _channels, PixelFormat & _hwFormat, uint32_t & _hwChannels ) const;

    protected:
        void onRenderImageDestroy_( OpenGLRenderImageES * _image );

    private:
        ConstString m_renderPlatform;

        mt::mat4f m_worldMatrix;
        mt::mat4f m_viewMatrix;
        mt::mat4f m_projectionMatrix;

        Resolution m_resolution;

        FactoryPtr m_factoryVertexBuffer;
        FactoryPtr m_factoryIndexBuffer;

        RenderIndexBufferInterfacePtr m_currentIndexBuffer;
        RenderVertexBufferInterfacePtr m_currentVertexBuffer;

        typedef stdex::map<ConstString, RenderFragmentShaderInterface *> TMapRenderFragmentShaders;
        TMapRenderFragmentShaders m_fragmentShaders;

        typedef stdex::map<ConstString, RenderVertexShaderInterface *> TMapRenderVertexShaders;
        TMapRenderVertexShaders m_vertexShaders;

        typedef stdex::vector<OpenGLRenderVertexShaderESPtr> TVectorRenderVertexShaders;
        TVectorRenderVertexShaders m_deferredCompileVertexShaders;

        typedef stdex::vector<OpenGLRenderFragmentShaderESPtr> TVectorRenderFragmentShaders;
        TVectorRenderFragmentShaders m_deferredCompileFragmentShaders;

        typedef stdex::vector<OpenGLRenderProgramESPtr> TVectorRenderPrograms;
        TVectorRenderPrograms m_deferredCompilePrograms;

        FactoryPtr m_factoryRenderImage;
        FactoryPtr m_factoryRenderFragmentShader;
        FactoryPtr m_factoryRenderVertexShader;
        FactoryPtr m_factoryRenderProgram;

        OpenGLRenderProgramESPtr m_currentProgram;

        uint32_t m_glMaxClipPlanes;
        uint32_t m_glMaxCombinedTextureImageUnits;

        TextureStage m_textureStage[MENGINE_MAX_TEXTURE_STAGES];

        typedef stdex::vector<OpenGLRenderImageES *> TVectorImages;
        TVectorImages m_images;

        bool m_renderWindowCreate;
        bool m_depthMask;
    };
}
