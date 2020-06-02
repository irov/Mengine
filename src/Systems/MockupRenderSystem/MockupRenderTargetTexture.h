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

    protected:
        uint32_t m_width;
        uint32_t m_height;
        uint32_t m_channels;
        EPixelFormat m_format;

        uint32_t m_hwWidth;
        uint32_t m_hwHeight;

        float m_hwWidthInv;
        float m_hwHeightInv;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MockupRenderTargetTexture> MockupRenderTargetTexturePtr;
    //////////////////////////////////////////////////////////////////////////
}