#pragma once

#include "Interface/RenderSystemInterface.h"
#include "Interface/DX9RenderSystemExtensionInterface.h"
#include "Interface/DynamicLibraryInterface.h"
#include "Interface/FactoryInterface.h"

#include "Environment/DirectX9/DirectX9RenderIncluder.h"

#include "DX9RenderResourceHandler.h"
#include "DX9RenderVertexBuffer.h"
#include "DX9RenderIndexBuffer.h"
#include "DX9RenderImageTarget.h"
#include "DX9RenderTargetTexture.h"
#include "DX9RenderTargetOffscreen.h"
#include "DX9RenderImage.h"
#include "DX9RenderVertexShader.h"
#include "DX9RenderFragmentShader.h"
#include "DX9RenderVertexAttribute.h"
#include "DX9RenderProgram.h"

#include "Kernel/IntrusiveList.h"
#include "Kernel/ServiceBase.h"
#include "Kernel/Vector.h"

namespace Mengine
{
    class DX9RenderSystem
        : public ServiceBase<RenderSystemInterface>
        , public DX9RenderSystemExtensionInterface
    {
        DECLARE_UNKNOWABLE();

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

        RenderProgramInterfacePtr createProgram( const ConstString & _name, const RenderVertexShaderInterfacePtr & _vertex, const RenderFragmentShaderInterfacePtr & _fragment, const RenderVertexAttributeInterfacePtr & _vertexAttribute, uint32_t _samplerCount, const DocumentPtr & _doc ) override;
        void setProgram( const RenderProgramInterfacePtr & _program ) override;
        void updateProgram( const RenderProgramInterfacePtr & _program ) override;
        RenderProgramVariableInterfacePtr createProgramVariable( uint32_t _vertexCount, uint32_t _pixelCount, const DocumentPtr & _doc ) override;
        bool setProgramVariable( const RenderProgramInterfacePtr & _program, const RenderProgramVariableInterfacePtr & _variable ) override;

    public:
        void drawIndexedPrimitive( const RenderMaterialStageCacheInterfacePtr & _stageCache, const RenderIndexedPrimitiveDesc & _desc ) override;

    public:
        void setTexture( const RenderProgramInterfacePtr & _program, uint32_t _stage, const RenderImageInterfacePtr & _image ) override;
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
        void clearFrameBuffer( uint32_t _frameBufferTypes, const Color & _color, double _depth, int32_t _stencil ) override;

        void setScissor( const Viewport & _viewport ) override;
        void removeScissor() override;

        void setViewport( const Viewport & _viewport ) override;

        void changeWindowMode( const Resolution & _resolution, bool _fullscreen ) override;

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
        IDirect3DDevice9 * getDirect3DDevice9() const override;

    protected:
        bool supportTextureFormat( EPixelFormat _format ) const;
        bool supportTextureNonPow2( const D3DCAPS9 & _d3dCaps ) const;

    protected:
        void updateVSyncDPP_();
        bool resetDevice_();

    protected:
        void updateViewport_( const Viewport & _viewport );

    protected:
        EPixelFormat updateImagePixelFormat_( EPixelFormat _pixelFormat ) const;

    protected:
        ConstString m_renderSystemName;

        DynamicLibraryInterfacePtr m_d3d9Library;

        Resolution m_windowResolution;
        Viewport m_windowViewport;
        bool m_fullscreen;
        bool m_depth;

        IDirect3D9 * m_pD3D;
        IDirect3DDevice9 * m_pD3DDevice;

        D3DCAPS9 m_d3dCaps;

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

    protected:
        void onDestroyDX9VertexAttribute_( DX9RenderVertexAttribute * _attribute );
        void onDestroyDX9VertexShader_( DX9RenderVertexShader * _shader );
        void onDestroyDX9FragmentShader_( DX9RenderFragmentShader * _shader );
        void onDestroyDX9Program_( DX9RenderProgram * _program );
        void onDestroyDX9VertexBuffer_( DX9RenderVertexBuffer * _buffer );
        void onDestroyDX9IndexBuffer_( DX9RenderIndexBuffer * _buffer );
        void onDestroyDX9RenderImage_( DX9RenderImage * _image );
        void onDestroyDX9RenderImageTarget_( DX9RenderImageTarget * _imageTarget );
        void onDestroyDX9RenderTargetTexture_( DX9RenderTargetTexture * _targetTexture );
        void onDestroyDX9RenderTargetOffscreen_( DX9RenderTargetOffscreen * _targetOffscreen );

    protected:
        void updateWVPInvMatrix_();

    protected:
        D3DMULTISAMPLE_TYPE findMatchingMultiSampleType_( uint32_t _MultiSampleCount );
        D3DFORMAT findMatchingZFormat_( const D3DCAPS9 & _d3dCaps, D3DFORMAT _backBufferFormat );

    protected:
        UINT m_adapterToUse;
        D3DDEVTYPE m_deviceType;

        Viewport m_viewport;

        FactoryInterfacePtr m_factoryRenderVertexAttribute;
        FactoryInterfacePtr m_factoryRenderVertexShader;
        FactoryInterfacePtr m_factoryRenderFragmentShader;
        FactoryInterfacePtr m_factoryRenderProgram;
        FactoryInterfacePtr m_factoryRenderProgramVariable;
        FactoryInterfacePtr m_factoryVertexBuffer;
        FactoryInterfacePtr m_factoryIndexBuffer;
        FactoryInterfacePtr m_factoryRenderImage;
        FactoryInterfacePtr m_factoryRenderImageTarget;
        FactoryInterfacePtr m_factoryRenderTargetTexture;
        FactoryInterfacePtr m_factoryRenderTargetOffscreen;

        typedef Vector<DX9RenderProgramPtr> VectorRenderPrograms;
        VectorRenderPrograms m_deferredCompilePrograms;

        typedef IntrusiveList<DX9RenderResourceHandler> IntrusiveListDX9RenderResourceHandler;
        IntrusiveListDX9RenderResourceHandler m_renderResourceHandlers;

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

        bool m_textureEnable[MENGINE_MAX_TEXTURE_STAGES];

        bool m_waitForVSync;
        bool m_lostDevice;

        bool m_supportA8;
        bool m_supportL8;
        bool m_supportR8G8B8;
        bool m_supportNonPow2;
    };
}
