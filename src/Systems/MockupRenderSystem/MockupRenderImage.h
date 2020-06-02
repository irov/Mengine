#pragma once

#include "Interface/RenderImageInterface.h"
#include "Interface/MemoryInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class MockupRenderImage
        : public RenderImageInterface
        , public Factorable
    {
    public:
        MockupRenderImage();
        ~MockupRenderImage() override;

    public:
        void initialize( ERenderImageMode _mode, uint32_t _mipmaps, uint32_t _hwWidth, uint32_t _hwHeight, uint32_t _hwChannels, uint32_t _hwDepth, EPixelFormat _hwPixelFormat );
        void finalize();

    public:
        void bind( uint32_t _stage ) override;

    protected:
        void setRenderImageProvider( const RenderImageProviderInterfacePtr & _renderImageProvider ) override;
        const RenderImageProviderInterfacePtr & getRenderImageProvider() const override;

    public:
        ERenderImageMode getMode() const override;

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
        Pointer lock( size_t * _pitch, uint32_t _level, const Rect & _rect, bool _readOnly ) override;
        bool unlock( uint32_t _level, bool _successful ) override;

    public:
        UnknownPointer getRenderImageExtention() override;

    protected:
        MemoryInterfacePtr m_memory;

        RenderImageProviderInterfacePtr m_renderImageProvider;

        ERenderImageMode m_mode;

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
