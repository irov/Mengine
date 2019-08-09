#pragma once

#include "Interface/RenderSystemInterface.h"

#include "DX9RenderResourceHandler.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Factory.h"

#include "Config/Vector.h"

#include "stdex/intrusive_list.h"

#include "d3d9.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class DX9RenderImage> DX9RenderImagePtr;
    typedef IntrusivePtr<class DX9RenderVertexShader> DX9RenderVertexShaderPtr;
    typedef IntrusivePtr<class DX9RenderFragmentShader> DX9RenderFragmentShaderPtr;
    typedef IntrusivePtr<class DX9RenderProgram> DX9RenderProgramPtr;
    //////////////////////////////////////////////////////////////////////////
    class DX9RenderImageTarget;
    class DX9RenderTargetTexture;
    class DX9RenderTargetOffscreen;
    //////////////////////////////////////////////////////////////////////////
    class DX9RenderSystem
        : public ServiceBase<RenderSystemInterface>
    {
    public:
        DX9RenderSystem();
        ~DX9RenderSystem() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        ERenderPlatform getRenderPlatformType() const override;
        const ConstString & getRenderPlatformName() const override;

    public:
        bool createRenderWindow( const Resolution & _resolution, uint32_t _bits, bool _fullscreen, bool _depth
            , bool _waitForVSync, int32_t _FSAAType, int32_t _FSAAQuality, uint32_t _MultiSampleCount ) override;

        void clear( uint8_t _r, uint8_t _g, uint8_t _b ) override;

    public:
        bool screenshot( const RenderImageInterfacePtr & _image, const mt::vec4f & _rect ) override;

    public:
        void setProjectionMatrix( const mt::mat4f & _projection ) override;
        void setViewMatrix( const mt::mat4f & _modelView ) override;
        void setTextureMatrix( uint32_t _stage, const mt::mat4f & _matrix ) override;
        void setWorldMatrix( const mt::mat4f & _world ) override;

    public:
        RenderVertexBufferInterfacePtr createVertexBuffer( uint32_t _vertexSize, EBufferType _bufferType, const Char * _doc ) override;
        bool setVertexBuffer( const RenderVertexBufferInterfacePtr & _vertexBuffer ) override;

        RenderIndexBufferInterfacePtr createIndexBuffer( uint32_t _indexSize, EBufferType _bufferType, const Char * _doc ) override;
        bool setIndexBuffer( const RenderIndexBufferInterfacePtr & _indexBuffer ) override;

    public:
        RenderVertexAttributeInterfacePtr createVertexAttribute( const ConstString & _name, uint32_t _elementSize, const Char * _doc ) override;
        RenderFragmentShaderInterfacePtr createFragmentShader( const ConstString & _name, const MemoryInterfacePtr & _memory, bool _compile, const Char * _doc ) override;
        RenderVertexShaderInterfacePtr createVertexShader( const ConstString & _name, const MemoryInterfacePtr & _memory, bool _compile, const Char * _doc ) override;

        RenderProgramInterfacePtr createProgram( const ConstString & _name, const RenderVertexShaderInterfacePtr & _vertex, const RenderFragmentShaderInterfacePtr & _fragment, const RenderVertexAttributeInterfacePtr & _vertexAttribute, uint32_t _samplerCount, const Char * _doc ) override;
        void setProgram( const RenderProgramInterfacePtr & _program ) override;
        void updateProgram( const RenderProgramInterfacePtr & _program ) override;
        RenderProgramVariableInterfacePtr createProgramVariable( uint32_t _vertexCount, uint32_t _pixelCount, const Char * _doc ) override;
        bool setProgramVariable( const RenderProgramVariableInterfacePtr & _variable, const RenderProgramInterfacePtr & _program ) override;

    public:
        void drawIndexedPrimitive( EPrimitiveType _type
            , uint32_t _baseVertexIndex
            , uint32_t _minIndex
            , uint32_t _vertexCount
            , uint32_t _startIndex
            , uint32_t _indexCount ) override;

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

        void setTextureStageFilter( uint32_t _stage, ETextureFilter _minification, ETextureFilter _mipmap, ETextureFilter _magnification ) override;

        // create empty render image
        RenderImageInterfacePtr createImage( uint32_t _mipmaps, uint32_t _width, uint32_t _height, uint32_t _channels, uint32_t _depth, EPixelFormat _format, const Char * _doc ) override;
        RenderImageInterfacePtr createDynamicImage( uint32_t _width, uint32_t _height, uint32_t _channels, uint32_t _depth, EPixelFormat _format, const Char * _doc ) override;


        // create render target image
        RenderTargetInterfacePtr createRenderTargetTexture( uint32_t _width, uint32_t _height, uint32_t _channels, EPixelFormat _format, const Char * _doc ) override;
        RenderTargetInterfacePtr createRenderTargetOffscreen( uint32_t _width, uint32_t _height, uint32_t _channels, EPixelFormat _format, const Char * _doc ) override;

        RenderImageInterfacePtr createRenderTargetImage( const RenderTargetInterfacePtr & _renderTarget, const Char * _doc ) override;

        // отрисовка изображения

        bool beginScene() override;
        void endScene() override;
        void swapBuffers() override;
        void clearFrameBuffer( uint32_t _frameBufferTypes, uint32_t _color, float _depth, uint32_t _stencil ) override;

        void setScissor( const Viewport & _viewport ) override;
        void removeScissor() override;

        void setViewport( const Viewport & _viewport ) override;

        void changeWindowMode( const Resolution & _resolution, bool _fullscreen ) override;

        bool supportTextureFormat( EPixelFormat _format ) const override;
        bool supportTextureNonPow2() const override;

        uint32_t getMaxCombinedTextureImageUnits() const override;

        void onWindowChangeFullscreenPrepare( bool _fullscreen ) override;
        void onWindowChangeFullscreen( bool _fullscreen ) override;
        void onWindowMovedOrResized() override;
        void onWindowClose() override;

        void setVSync( bool _vSync ) override;

    public:
        uint32_t getTextureMemoryUse() const override;
        uint32_t getTextureCount() const override;

    protected:
        void updateVSyncDPP_();
        bool resetDevice_();

    protected:
        void updateViewport_( const Viewport & _viewport );

    protected:
        void fixNPOTSupport_( uint32_t & _width, uint32_t & _height ) const;

    private:
        ConstString m_renderPlatformName;

        HMODULE m_hd3d9;

        Resolution m_windowResolution;
        Viewport m_windowViewport;
        bool m_fullscreen;
        bool m_depth;

        IDirect3D9 * m_pD3D;
        IDirect3DDevice9 * m_pD3DDevice;

        D3DCAPS9 m_caps;

        D3DPRESENT_PARAMETERS m_d3dppW;
        D3DPRESENT_PARAMETERS m_d3dppFS;

        D3DDISPLAYMODE m_displayMode;

        D3DPRESENT_PARAMETERS * m_d3dpp;

        // sync routines
        uint32_t m_frames;

    protected:
        bool releaseResources_();
        void release_();
        bool restore_();

        //void createSyncTargets_();

        bool d3dCreateTexture_( uint32_t Width, uint32_t Height, uint32_t MipLevels
            , DWORD Usage, EPixelFormat Format, D3DPOOL Pool, LPDIRECT3DTEXTURE9 * _ppD3DTexture );

        DX9RenderImagePtr createDX9RenderImage_( LPDIRECT3DTEXTURE9 _pD3DTexture, ERenderImageMode _mode
            , uint32_t _mipmaps, uint32_t _hwWidth, uint32_t _hwHeight, uint32_t _hwChannels, uint32_t _hwDepth, EPixelFormat _hwPixelFormat, const Char * _doc );

    protected:
        void onDestroyDX9RenderImage_( DX9RenderImage * _image );
        void onDestroyDX9RenderImageTarget_( DX9RenderImageTarget * _imageTarget );
        void onDestroyDX9RenderTargetTexture_( DX9RenderTargetTexture * _targetTexture );
        void onDestroyDX9RenderTargetOffscreen_( DX9RenderTargetOffscreen * _targetOffscreen );

    protected:
        void updateWVPInvMatrix_();

    protected:
        D3DMULTISAMPLE_TYPE findMatchingMultiSampleType_( uint32_t _MultiSampleCount );
        D3DFORMAT findMatchingZFormat_( D3DFORMAT _backBufferFormat );

    protected:
        UINT m_adapterToUse;
        D3DDEVTYPE m_deviceType;

        Viewport m_viewport;

        IDirect3DVertexDeclaration9 * m_vertexDeclaration;

        FactoryPtr m_factoryRenderVertexAttribute;
        FactoryPtr m_factoryRenderVertexShader;
        FactoryPtr m_factoryRenderFragmentShader;
        FactoryPtr m_factoryRenderProgram;
        FactoryPtr m_factoryRenderProgramVariable;
        FactoryPtr m_factoryVertexBuffer;
        FactoryPtr m_factoryIndexBuffer;
        FactoryPtr m_factoryDX9Image;
        FactoryPtr m_factoryDX9ImageTarget;
        FactoryPtr m_factoryDX9TargetTexture;
        FactoryPtr m_factoryDX9TargetOffscreen;

        typedef Vector<DX9RenderVertexShaderPtr> VectorRenderVertexShaders;
        VectorRenderVertexShaders m_deferredCompileVertexShaders;

        typedef Vector<DX9RenderFragmentShaderPtr> VectorRenderFragmentShaders;
        VectorRenderFragmentShaders m_deferredCompileFragmentShaders;

        typedef Vector<DX9RenderProgramPtr> VectorRenderPrograms;
        VectorRenderPrograms m_deferredCompilePrograms;

        typedef stdex::intrusive_list<DX9RenderResourceHandler> IntrusiveListDX9RenderResourceHandler;
        IntrusiveListDX9RenderResourceHandler m_renderResourceHandlers;

        mt::mat4f m_projectionMatrix;
        mt::mat4f m_modelViewMatrix;
        mt::mat4f m_worldMatrix;
        mt::mat4f m_totalWVPInvMatrix;

        uint32_t m_dxMaxCombinedTextureImageUnits;

        uint32_t m_textureMemoryUse;
        uint32_t m_textureCount;

        bool m_vertexBufferEnable;
        bool m_indexBufferEnable;

        bool m_textureEnable[MENGINE_MAX_TEXTURE_STAGES];

        bool m_waitForVSync;
    };
}
