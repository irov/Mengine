#pragma once

#include "Interface/RenderSystemInterface.h"
#include "Interface/DX11RenderSystemExtensionInterface.h"

#include "Environment/DirectX11/DirectX11RenderIncluder.h"

#include "DX11RenderResourceHandler.h"

#include "Kernel/IntrusiveList.h"
#include "Kernel/ServiceBase.h"
#include "Kernel/Factory.h"
#include "Kernel/Vector.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class DX11RenderImage, class RenderImageInterface> DX11RenderImagePtr;
    typedef IntrusivePtr<class DX11RenderVertexShader, class RenderVertexShaderInterface> DX11RenderVertexShaderPtr;
    typedef IntrusivePtr<class DX11RenderFragmentShader, class RenderFragmentShaderInterface> DX11RenderFragmentShaderPtr;
    typedef IntrusivePtr<class DX11RenderVertexAttribute, class RenderVertexAttributeInterface> DX11RenderVertexAttributePtr;
    typedef IntrusivePtr<class DX11RenderProgram, class RenderProgramInterface> DX11RenderProgramPtr;
    //////////////////////////////////////////////////////////////////////////
    class DX11RenderVertexBuffer;
    class DX11RenderIndexBuffer;
    class DX11RenderImageTarget;
    class DX11RenderTargetTexture;
    class DX11RenderTargetOffscreen;
    class DX11RenderMaterialStageCache;
    //////////////////////////////////////////////////////////////////////////
    class DX11RenderSystem
        : public ServiceBase<RenderSystemInterface>
        , public DX11RenderSystemExtensionInterface
    {
        DECLARE_UNKNOWABLE();

    public:
        DX11RenderSystem();
        ~DX11RenderSystem() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        ERenderPlatform getRenderPlatformType() const override;
        const ConstString & getRenderPlatformName() const override;

    public:
        bool createRenderWindow( const RenderWindowDesc * _windowDesc ) override;

    public:
        void setProjectionMatrix( const mt::mat4f & _projection ) override;
        void setViewMatrix( const mt::mat4f & _modelView ) override;
        void setTextureMatrix( uint32_t _stage, const mt::mat4f & _matrix ) override;
        void setWorldMatrix( const mt::mat4f & _world ) override;

    public:
        RenderVertexBufferInterfacePtr createVertexBuffer( uint32_t _vertexSize, EBufferType _bufferType, const DocumentPtr & _doc ) override;
        bool setVertexBuffer( const RenderVertexBufferInterfacePtr & _vertexBuffer ) override;

        RenderIndexBufferInterfacePtr createIndexBuffer( uint32_t _indexSize, EBufferType _bufferType, const DocumentPtr & _doc ) override;
        bool setIndexBuffer( const RenderIndexBufferInterfacePtr & _indexBuffer ) override;

    public:
        RenderVertexAttributeInterfacePtr createVertexAttribute( const ConstString & _name, uint32_t _elementSize, const DocumentPtr & _doc ) override;
        RenderFragmentShaderInterfacePtr createFragmentShader( const ConstString & _name, const MemoryInterfacePtr & _memory, bool _compile, const DocumentPtr & _doc ) override;
        RenderVertexShaderInterfacePtr createVertexShader( const ConstString & _name, const MemoryInterfacePtr & _memory, bool _compile, const DocumentPtr & _doc ) override;

        RenderProgramInterfacePtr createProgram( const ConstString & _name, const RenderVertexShaderInterfacePtr & _vertexShader, const RenderFragmentShaderInterfacePtr & _fragmentShader, const RenderVertexAttributeInterfacePtr & _vertexAttribute, uint32_t _samplerCount, const DocumentPtr & _doc ) override;
        void setProgram( const RenderProgramInterfacePtr & _program ) override;
        void updateProgram( const RenderProgramInterfacePtr & _program ) override;
        RenderProgramVariableInterfacePtr createProgramVariable( uint32_t _vertexCount, uint32_t _pixelCount, const DocumentPtr & _doc ) override;
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
        RenderImageInterfacePtr createImage( uint32_t _mipmaps, uint32_t _width, uint32_t _height, EPixelFormat _format, const DocumentPtr & _doc ) override;

        RenderTargetInterfacePtr createRenderTargetTexture( uint32_t _width, uint32_t _height, EPixelFormat _format, const DocumentPtr & _doc ) override;
        RenderTargetInterfacePtr createRenderTargetOffscreen( uint32_t _width, uint32_t _height, EPixelFormat _format, const DocumentPtr & _doc ) override;

        RenderImageInterfacePtr createRenderImageTarget( const RenderTargetInterfacePtr & _renderTarget, const DocumentPtr & _doc ) override;

    public:
        RenderMaterialStageCacheInterfacePtr createRenderMaterialStageCache( const RenderMaterialStage * _stage, const DocumentPtr & _doc ) override;

    public:
        bool beginScene() override;
        void endScene() override;
        void swapBuffers() override;
        void clearFrameBuffer( uint32_t _frameBufferTypes, const Color & _color, float _depth, uint32_t _stencil ) override;

        void setScissor( const Viewport & _viewport ) override;
        void removeScissor() override;

        void setViewport( const Viewport & _viewport ) override;

        void changeWindowMode( const Resolution & _resolution, bool _fullscreen ) override;

        bool supportTextureFormat( EPixelFormat _format ) const override;
        bool supportTextureNonPow2() const override;

        uint32_t getMaxCombinedTextureImageUnits() const override;

        void onWindowChangeFullscreenPrepare( bool _fullscreen ) override;
        bool onWindowChangeFullscreen( bool _fullscreen ) override;
        void onWindowMovedOrResized() override;
        void onWindowClose() override;

        void setVSync( bool _vSync ) override;

    public:
        uint32_t getAvailableTextureMemory() const override;
        uint32_t getTextureMemoryUse() const override;
        uint32_t getTextureCount() const override;

    public:
        const ID3D11DevicePtr & getDirect3D11Device() const override;
        const ID3D11DeviceContextPtr & getDirect3D11DeviceContext() const override;

    protected:
        bool createSwapChain_( IDXGIFactory2 * _dxgiFactory );
        bool createRenderTargetView_();
        bool createDepthStencilBuffer_();
        bool createDepthStencilView_();

    protected:
        bool updateRasterizerState_() const;

    protected:
        void updateVSyncDPP_( UINT _width, UINT _height, DXGI_RATIONAL * const _refreshRate );
        bool resetDevice_();

    protected:
        void updateViewport_( const Viewport & _viewport );

    protected:
        void fixNPOTSupport_( uint32_t * const _width, uint32_t * const _height ) const;

    private:
        ConstString m_renderSystemName;

        Resolution m_windowResolution;
        Viewport m_windowViewport;
        bool m_windowFullscreen;
        bool m_windowDepth;

        ID3D11DevicePtr m_pD3DDevice;
        ID3D11DeviceContextPtr m_pD3DDeviceContext;
        ID3D11DeviceContextPtr m_pD3DDeviceImmediateContext;

        DXGI_MODE_DESC m_dxgiSwapChainBufferDesc;
        IDXGISwapChainPtr m_dxgiSwapChain;        

        typedef Vector<DXGI_MODE_DESC> VectorModeDescs;
        VectorModeDescs m_DisplayModeList;

        ID3D11RenderTargetViewPtr m_renderTargetView;

        ID3D11Texture2DPtr m_depthStencilBuffer;
        ID3D11DepthStencilViewPtr m_depthStencilView;

        // sync routines
        uint32_t m_frames;

        D3D11_RASTERIZER_DESC m_D3DRasterizerStateDesc;

        mutable ID3D11RasterizerStatePtr m_pD3DRasterizerState;
        mutable bool m_invalidateRasterizerState;

    protected:
        bool releaseResources_();
        bool restore_();

    protected:
        void onDestroyVertexAttribute_( DX11RenderVertexAttribute * _attribute );
        void onDestroyVertexShader_( DX11RenderVertexShader * _shader );
        void onDestroyFragmentShader_( DX11RenderFragmentShader * _shader );
        void onDestroyProgram_( DX11RenderProgram * _program );
        void onDestroyVertexBuffer_( DX11RenderVertexBuffer * _buffer );
        void onDestroyIndexBuffer_( DX11RenderIndexBuffer * _buffer );
        void onDestroyRenderImage_( DX11RenderImage * _image );
        void onDestroyRenderImageTarget_( DX11RenderImageTarget * _imageTarget );
        void onDestroyRenderTargetTexture_( DX11RenderTargetTexture * _targetTexture );
        void onDestroyRenderTargetOffscreen_( DX11RenderTargetOffscreen * _targetOffscreen );
        void onDestroyRenderMaterialStageCache_( DX11RenderMaterialStageCache * _materialStageCache );

    protected:
        void updateWVPInvMatrix_();

    protected:
        UINT m_adapterToUse;
        uint32_t m_multiSampleCount;

        Viewport m_viewport;

        FactoryPtr m_factoryRenderVertexAttribute;
        FactoryPtr m_factoryRenderVertexShader;
        FactoryPtr m_factoryRenderFragmentShader;
        FactoryPtr m_factoryRenderProgram;
        FactoryPtr m_factoryRenderProgramVariable;
        FactoryPtr m_factoryVertexBuffer;
        FactoryPtr m_factoryIndexBuffer;
        FactoryPtr m_factoryRenderImage;
        FactoryPtr m_factoryRenderImageTarget;
        FactoryPtr m_factoryRenderTargetTexture;
        FactoryPtr m_factoryRenderTargetOffscreen;
        FactoryPtr m_factoryRenderMaterialStageCache;

        typedef Vector<DX11RenderProgramPtr> VectorRenderPrograms;
        VectorRenderPrograms m_deferredCompilePrograms;

        typedef IntrusiveList<DX11RenderResourceHandler> IntrusiveListRenderResourceHandler;
        IntrusiveListRenderResourceHandler m_renderResourceHandlers;

        mt::mat4f m_projectionMatrix;
        mt::mat4f m_modelViewMatrix;
        mt::mat4f m_worldMatrix;
        mt::mat4f m_totalWVPInvMatrix;

        uint32_t m_textureMemoryUse;
        uint32_t m_textureCount;
        uint32_t m_vertexBufferCount;
        uint32_t m_indexBufferCount;

        bool m_vertexBufferEnable;
        bool m_indexBufferEnable;
        bool m_vertexAttributeEnable;
        bool m_vertexShaderEnable;
        bool m_fragmentShaderEnable;

        bool m_textureEnable[MENGINE_MAX_TEXTURE_STAGES] = {false};

        bool m_waitForVSync;
    };
    //////////////////////////////////////////////////////////////////////////
}
