#pragma once

#include "Interface/RenderTargetInterface.h"

#include "Environment/OpenGL/OpenGLRenderIncluder.h"

#include "OpenGLRenderResourceHandler.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class OpenGLRenderTargetTexture
        : public RenderTargetInterface
        , public OpenGLRenderResourceHandler
        , public Factorable
    {
        DECLARE_FACTORABLE( OpenGLRenderTargetTexture );

    public:
        OpenGLRenderTargetTexture();
        ~OpenGLRenderTargetTexture() override;

    public:
        bool initialize( uint32_t _width, uint32_t _height, uint32_t _channels, EPixelFormat _pixelFormat, GLint _internalFormat, GLenum _format, GLenum _type );
        void finalize();

    public:
        bool create();
        void release();
        bool reload();

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
        bool begin() const override;
        void end() const override;

    public:
        bool getData( void * const _buffer, size_t _pitch ) const override;

    public:
        GLuint getUID() const;

    public:
        void onRenderReset() override;
        bool onRenderRestore() override;

    protected:
        GLuint m_tuid;
        GLuint m_fuid;

        uint32_t m_width;
        uint32_t m_height;

        uint32_t m_hwWidth;
        uint32_t m_hwHeight;
        uint32_t m_hwChannels;

        float m_hwWidthInv;
        float m_hwHeightInv;

        EPixelFormat m_hwPixelFormat;

        GLenum m_minFilter;
        GLenum m_magFilter;
        GLenum m_wrapS;
        GLenum m_wrapT;

        GLint m_internalFormat;

        GLenum m_format;
        GLenum m_type;

        bool m_pow2;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<OpenGLRenderTargetTexture, RenderTargetInterface> OpenGLRenderTargetTexturePtr;
    //////////////////////////////////////////////////////////////////////////
}