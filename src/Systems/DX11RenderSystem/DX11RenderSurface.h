#pragma once

#include "Interface/RenderSurfaceInterface.h"

#include "Environment/DirectX11/DX11RenderIncluder.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum EDX11PresentResult
    {
        EDX11PR_SUCCESSFUL,
        EDX11PR_RECOVERABLE,
        EDX11PR_DEVICE_LOST
    };
    //////////////////////////////////////////////////////////////////////////
    class DX11RenderSurface
        : public RenderSurfaceInterface
    {
        DECLARE_FACTORABLE( DX11RenderSurface );

    public:
        DX11RenderSurface();
        ~DX11RenderSurface() override;

    public:
        bool initialize( ID3D11Device * _device, void * _nativeHandle, const Resolution & _resolution, float _dpiScale );
        void finalize();

    public:
        const Resolution & getResolution() const override;
        float getDpiScale() const override;

    public:
        bool resize( const Resolution & _resolution, float _dpiScale ) override;
        void setVSync( bool _vsync ) override;

    public:
        bool setFullscreen( bool _fullscreen );
        bool resizeTarget( const DXGI_MODE_DESC & _modeDesc );

    public:
        bool acquire();
        bool bind( ID3D11DeviceContext * _context );
        void unbind( ID3D11DeviceContext * _context );
        EDX11PresentResult present();

    public:
        ID3D11RenderTargetView * getColorTarget() const;
        ID3D11DepthStencilView * getDepthTarget() const;

    protected:
        bool createTargets_();
        void releaseTargets_();

    protected:
        Resolution m_resolution;
        float m_dpiScale;
        bool m_waitForVSync;
        ID3D11DevicePtr m_device;
        IDXGISwapChain1Ptr m_swapChain;
        ID3D11RenderTargetViewPtr m_colorTarget;
        ID3D11Texture2DPtr m_depthTexture;
        ID3D11DepthStencilViewPtr m_depthTarget;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DX11RenderSurface, RenderSurfaceInterface> DX11RenderSurfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
