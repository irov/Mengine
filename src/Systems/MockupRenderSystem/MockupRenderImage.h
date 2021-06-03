#pragma once

#include "Interface/RenderImageInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class MockupRenderImage
        : public RenderImageInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( MockupRenderImage );

    public:
        MockupRenderImage();
        ~MockupRenderImage() override;

    public:
        void initialize( uint32_t _mipmaps, uint32_t _hwWidth, uint32_t _hwHeight, EPixelFormat _hwPixelFormat );
        void finalize();

    protected:
        void setRenderImageProvider( const RenderImageProviderInterfacePtr & _renderImageProvider ) override;
        const RenderImageProviderInterfacePtr & getRenderImageProvider() const override;

    public:
        uint32_t getHWWidth() const override;
        uint32_t getHWHeight() const override;
        uint32_t getHWMipmaps() const override;

        EPixelFormat getHWPixelFormat() const override;

    protected:
        float getHWWidthInv() const override;
        float getHWHeightInv() const override;

    public:
        RenderImageLockedInterfacePtr lock( uint32_t _level, const Rect & _rect, bool _readOnly ) override;
        bool unlock( const RenderImageLockedInterfacePtr & _locked, uint32_t _level, bool _successful ) override;

    protected:
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
    typedef IntrusivePtr<MockupRenderImage> MockupRenderImagePtr;
    //////////////////////////////////////////////////////////////////////////
}
