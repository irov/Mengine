#pragma once

#include "Interface/RenderTargetInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class MockupRenderTargetTexture
        : public RenderTargetInterface
        , public Factorable
    {
    public:
        MockupRenderTargetTexture();
        ~MockupRenderTargetTexture() override;

    public:
        bool initialize( uint32_t _width, uint32_t _height, uint32_t _channels, EPixelFormat _format );
        void finalize();

    protected:
        virtual bool _initialize();
        virtual void _finalize();

    public:
        uint32_t getHWMipmaps() const override;
        uint32_t getHWWidth() const override;
        uint32_t getHWHeight() const override;
        uint32_t getHWChannels() const override;
        uint32_t getHWDepth() const override;
        EPixelFormat getHWPixelFormat() const override;

    public:
        float getHWWidthInv() const override;
        float getHWHeightInv() const override;

    public:
        void calcViewport( const mt::vec2f & _size, Viewport * const _viewport ) const override;

    public:
        bool begin() override;
        void end() override;

    public:
        bool getData( void * const _buffer, size_t _pitch ) const override;

    protected:
        uint32_t m_width;
        uint32_t m_height;
        uint32_t m_hwChannels;
        EPixelFormat m_hwPixelFormat;

        uint32_t m_hwWidth;
        uint32_t m_hwHeight;

        float m_hwWidthInv;
        float m_hwHeightInv;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MockupRenderTargetTexture> MockupRenderTargetTexturePtr;
    //////////////////////////////////////////////////////////////////////////
}