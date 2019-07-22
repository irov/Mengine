#pragma once

#include "Interface/RenderTargetInterface.h"

#include "DX9RenderResourceHandler.h"

#include "Kernel/Factorable.h"

#include "d3d9.h"

namespace Mengine
{
    class DX9RenderTargetTexture
        : public RenderTargetInterface
        , public Factorable
        , public DX9RenderResourceHandler
    {
    public:
        DX9RenderTargetTexture();
        ~DX9RenderTargetTexture() override;

    public:
        bool initialize( LPDIRECT3DDEVICE9 _device, uint32_t _width, uint32_t _height, uint32_t _channels, EPixelFormat _format );
        void finalize();

    protected:
        virtual bool _initialize();
        virtual void _finalize();

    public:
        uint32_t getWidth() const override;
        uint32_t getHeight() const override;
        uint32_t getChannels() const override;
        uint32_t getDepth() const override;
        EPixelFormat getPixelFormat() const override;

    public:
        uint32_t getHWWidth() const override;
        uint32_t getHWHeight() const override;

    public:
        float getHWWidthInv() const override;
        float getHWHeightInv() const override;

    public:
        bool begin() override;
        void end() override;

    public:
        bool getData( void * _buffer, size_t _pitch ) override;

    public:
        LPDIRECT3DDEVICE9 getDirect3dDevice9() const;
        LPDIRECT3DTEXTURE9 getDirect3dTexture9() const;

    protected:
        void onRenderReset() override;
        bool onRenderRestore() override;

    protected:
        uint32_t m_width;
        uint32_t m_height;
        uint32_t m_channels;
        EPixelFormat m_format;

        uint32_t m_hwWidth;
        uint32_t m_hwHeight;

        float m_hwWidthInv;
        float m_hwHeightInv;

        LPDIRECT3DDEVICE9 m_pD3DDevice;
        LPDIRECT3DTEXTURE9 m_pD3DTexture;
        LPDIRECT3DSURFACE9 m_pD3DSurface;
        LPDIRECT3DSURFACE9 m_pD3DSurfaceOld;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DX9RenderTargetTexture> DX9RenderTargetTexturePtr;
    //////////////////////////////////////////////////////////////////////////
}