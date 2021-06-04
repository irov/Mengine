#pragma once

#include "Interface/RenderImageInterface.h"
#include "Interface/DX11RenderImageExtensionInterface.h"

#include "Environment/DirectX11/DirectX11RenderIncluder.h"

#include "DX11RenderResourceHandler.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DX11RenderImage
        : public RenderImageInterface
        , public DX11RenderImageExtensionInterface        
        , public DX11RenderResourceHandler
        , public Factorable
    {
        DECLARE_FACTORABLE( DX11RenderImage );
        DECLARE_UNKNOWABLE();

    public:
        DX11RenderImage();
        ~DX11RenderImage() override;

    public:
        bool initialize( uint32_t _mipmaps, uint32_t _width, uint32_t _height, EPixelFormat _pixelFormat );
        void finalize();

    public:
        void bind( const ID3D11DeviceContextPtr & _pImmediateContext, uint32_t _stage ) override;
        void unbind( const ID3D11DeviceContextPtr & _pImmediateContext, uint32_t _stage ) override;

    public:
        void setRenderImageProvider( const RenderImageProviderInterfacePtr & _renderImageProvider ) override;
        const RenderImageProviderInterfacePtr & getRenderImageProvider() const override;

    public:
        uint32_t getHWWidth() const override;
        uint32_t getHWHeight() const override;
        uint32_t getHWMipmaps() const override;

        EPixelFormat getHWPixelFormat() const override;

    public:
        float getHWWidthInv() const override;
        float getHWHeightInv() const override;

    public:
        bool getUpscalePow2() const override;

    public:
        RenderImageLockedInterfacePtr lock( uint32_t _level, const Rect & _rect, bool _readOnly ) override;
        bool unlock( const RenderImageLockedInterfacePtr & _locked, uint32_t _level, bool _successful ) override;

    protected:
        void onRenderReset() override;
        bool onRenderRestore() override;

    public:
        const ID3D11DevicePtr & getD3DDevice() const override;
        const ID3D11Texture2DPtr & getD3DTexture() const override;
        const ID3D11ShaderResourceViewPtr & getD3DShaderResource() const override;

    protected:
        ID3D11Texture2DPtr m_pD3DTexture;

        // to bind texture to shaders
        ID3D11ShaderResourceViewPtr m_pD3DResourceView;

        RenderImageProviderInterfacePtr m_renderImageProvider;

        EPixelFormat m_hwPixelFormat;

        uint32_t m_hwMipmaps;
        uint32_t m_hwWidth;
        uint32_t m_hwHeight;
        uint32_t m_hwChannels;
        uint32_t m_hwDepth;

        float m_hwWidthInv;
        float m_hwHeightInv;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DX11RenderImage, RenderImageInterface> DX11RenderImagePtr;
    //////////////////////////////////////////////////////////////////////////
}
