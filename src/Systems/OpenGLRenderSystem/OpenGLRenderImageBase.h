#pragma once

#include "Interface/RenderImageInterface.h"

#include "Environment/OpenGL/OpenGLRenderIncluder.h"
#include "Environment/OpenGL/OpenGLRenderImageExtensionInterface.h"

#include "OpenGLRenderResourceHandler.h"

#include "Kernel/Factorable.h"

#include "Config/Timestamp.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class OpenGLRenderImageBase
        : public RenderImageInterface
        , public OpenGLRenderImageExtensionInterface
        , public OpenGLRenderResourceHandler
        , public Factorable
    {
        DECLARE_UNKNOWABLE();

    public:
        OpenGLRenderImageBase();
        ~OpenGLRenderImageBase() override;

    public:
        bool initialize( uint32_t _mipmaps, uint32_t _width, uint32_t _height, uint32_t _layers, EPixelFormat _pixelFormat, GLint _internalFormat, GLenum _format, GLenum _type );
        void finalize();

    public:
        uint32_t getHWLayerCount() const override;

    public:
        void bind( uint32_t _stage ) override;
        void unbind( uint32_t _stage ) override;

    public:
        void setRenderImageProvider( const RenderImageProviderInterfacePtr & _renderImageProvider ) override;
        const RenderImageProviderInterfacePtr & getRenderImageProvider() const override;

    public:
        bool create();
        void release();

    protected:
        virtual bool _create() = 0;

    public:
        RenderImageLockedInterfacePtr lock( uint32_t _layer, uint32_t _level, const Rect & _rect, bool _readOnly ) override;
        bool unlock( const RenderImageLockedInterfacePtr & _locked, uint32_t _layer, uint32_t _level, bool _successful ) override;

    protected:
        virtual bool _unlock( const Rect & _lockedRect, const void * _buffer, uint32_t _layer, uint32_t _level ) = 0;

    public:
        uint32_t getHWMipmaps() const override;
        uint32_t getHWWidth() const override;
        uint32_t getHWHeight() const override;

    public:
        EPixelFormat getHWPixelFormat() const override;

    public:
        float getHWWidthInv() const override;
        float getHWHeightInv() const override;

    public:
        bool getUpscalePow2() const override;

    public:
        GLuint getUID() const override;
        virtual GLenum getTextureTarget() const override = 0;

    public:
        void setMinFilter( GLenum _minFilter );
        GLenum getMinFilter() const;

        void setMagFilter( GLenum _magFilter );
        GLenum getMagFilter() const;

        void setWrapS( GLenum _wrapS );
        GLenum getWrapS() const;

        void setWrapT( GLenum _wrapT );
        GLenum getWrapT() const;

    protected:
        void onRenderReset() override;
        bool onRenderRestore() override;

    protected:
        RenderImageProviderInterfacePtr m_renderImageProvider;

        Timestamp m_createTimestamp;

        GLuint m_uid;

        EPixelFormat m_hwPixelFormat;

        uint32_t m_width;
        uint32_t m_height;

        uint32_t m_hwMipmaps;
        uint32_t m_hwWidth;
        uint32_t m_hwHeight;
        uint32_t m_hwLayers;

        float m_hwWidthInv;
        float m_hwHeightInv;

        GLenum m_minFilter;
        GLenum m_magFilter;
        GLenum m_wrapS;
        GLenum m_wrapT;

        GLint m_internalFormat;

        GLenum m_format;
        GLenum m_type;

        bool m_pow2;
        bool m_upscalePow2;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<OpenGLRenderImageBase, RenderImageInterface> OpenGLRenderImageBasePtr;
    //////////////////////////////////////////////////////////////////////////
}
