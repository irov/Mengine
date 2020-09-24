#pragma once

#include "Interface/RenderImageInterface.h"
#include "Interface/DX9RenderImageExtensionInterface.h"

#include "DX9RenderResourceHandler.h"

#include "Kernel/Factorable.h"

#include "d3d9.h"

namespace Mengine
{
    class DX9RenderImage
        : public RenderImageInterface
        , public DX9RenderImageExtensionInterface
        , public Factorable
        , public DX9RenderResourceHandler
    {
    public:
        DX9RenderImage();
        ~DX9RenderImage() override;

    public:
        void setDirect3DDevice9( IDirect3DDevice9 * _pD3DDevice );
        IDirect3DDevice9 * getDirect3DDevice9() const;

    public:
        bool initialize( uint32_t _mipmaps, uint32_t _width, uint32_t _height, uint32_t _channels, uint32_t _depth, EPixelFormat _pixelFormat );
        void finalize();

    public:
        void bind( uint32_t _stage ) override;
        void unbind( uint32_t _stage ) override;

    protected:
        void setRenderImageProvider( const RenderImageProviderInterfacePtr & _renderImageProvider ) override;
        const RenderImageProviderInterfacePtr & getRenderImageProvider() const override;

    public:
        uint32_t getHWWidth() const override;
        uint32_t getHWHeight() const override;
        uint32_t getHWChannels() const override;
        uint32_t getHWDepth() const override;
        uint32_t getHWMipmaps() const override;

        EPixelFormat getHWPixelFormat() const override;

    protected:
        float getHWWidthInv() const override;
        float getHWHeightInv() const override;

    public:
        Pointer lock( size_t * const _pitch, uint32_t _level, const Rect & _rect, bool _readOnly ) override;
        bool unlock( uint32_t _level, bool _successful ) override;

    protected:
        void onRenderReset() override;
        bool onRenderRestore() override;

    public:
        UnknownPointer getRenderImageExtention() override;

    public:
        IDirect3DDevice9 * getD3DDevice() const override;
        IDirect3DTexture9 * getD3DTexture() const override;

    protected:
        IDirect3DDevice9 * m_pD3DDevice;
        IDirect3DTexture9 * m_pD3DTexture;

        RenderImageProviderInterfacePtr m_renderImageProvider;

        uint32_t m_hwMipmaps;
        uint32_t m_hwWidth;
        uint32_t m_hwHeight;
        uint32_t m_hwChannels;
        uint32_t m_hwDepth;

        EPixelFormat m_hwPixelFormat;

        float m_hwWidthInv;
        float m_hwHeightInv;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DX9RenderImage> DX9RenderImagePtr;
}
