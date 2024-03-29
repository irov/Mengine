#pragma once

#include "Interface/RenderTargetInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class MockupRenderTargetTexture
        : public RenderTargetInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( MockupRenderTargetTexture );

    public:
        MockupRenderTargetTexture();
        ~MockupRenderTargetTexture() override;

    public:
        bool initialize( uint32_t _width, uint32_t _height, EPixelFormat _format );
        void finalize();

    protected:
        virtual bool _initialize();
        virtual void _finalize();

    public:
        uint32_t getHWMipmaps() const override;
        uint32_t getHWWidth() const override;
        uint32_t getHWHeight() const override;
        EPixelFormat getHWPixelFormat() const override;

    public:
        float getHWWidthInv() const override;
        float getHWHeightInv() const override;

    public:
        bool getUpscalePow2() const override;

    public:
        bool begin() const override;
        void end() const override;

    public:
        const mt::uv4f & getUV() const override;

    public:
        bool getData( void * const _buffer, size_t _pitch ) const override;

    protected:
        uint32_t m_hwWidth;
        uint32_t m_hwHeight;

        EPixelFormat m_hwPixelFormat;

        float m_hwWidthInv;
        float m_hwHeightInv;

        mt::uv4f m_uv;

        bool m_upscalePow2;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MockupRenderTargetTexture> MockupRenderTargetTexturePtr;
    //////////////////////////////////////////////////////////////////////////
}