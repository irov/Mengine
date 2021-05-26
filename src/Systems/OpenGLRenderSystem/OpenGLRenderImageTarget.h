#pragma once

#include "Interface/RenderImageInterface.h"
#include "Interface/OpenGLRenderImageExtensionInterface.h"

#include "Environment/OpenGL/OpenGLRenderIncluder.h"

#include "OpenGLRenderTargetTexture.h"
#include "OpenGLRenderResourceHandler.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class OpenGLRenderImageTarget
        : public RenderImageInterface
        , public OpenGLRenderImageExtensionInterface
        , public OpenGLRenderResourceHandler
        , public Factorable
    {
        DECLARE_FACTORABLE( OpenGLRenderImageTarget );
        DECLARE_UNKNOWABLE();

    public:
        OpenGLRenderImageTarget();
        ~OpenGLRenderImageTarget() override;

    public:
        bool initialize( const OpenGLRenderTargetTexturePtr & _renderTarget );
        void finalize();

    public:
        void bind( uint32_t _stage ) override;
        void unbind( uint32_t _stage ) override;

    protected:
        void setRenderImageProvider( const RenderImageProviderInterfacePtr & _renderImageProvider ) override;
        const RenderImageProviderInterfacePtr & getRenderImageProvider() const override;

    protected:
        uint32_t getHWWidth() const override;
        uint32_t getHWHeight() const override;
        uint32_t getHWChannels() const override;
        uint32_t getHWDepth() const override;
        uint32_t getHWMipmaps() const override;

        EPixelFormat getHWPixelFormat() const override;

    protected:
        float getHWWidthInv() const override;
        float getHWHeightInv() const override;

    protected:
        RenderImageLockedInterfacePtr lock( uint32_t _level, const Rect & _rect, bool _readOnly ) override;
        bool unlock( const RenderImageLockedInterfacePtr & _locked, uint32_t _level, bool _successful ) override;

    protected:
        GLuint getUID() const override;

    protected:
        void onRenderReset() override;
        bool onRenderRestore() override;

    protected:
        OpenGLRenderTargetTexturePtr m_renderTarget;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<OpenGLRenderImageTarget, RenderImageInterface> OpenGLRenderImageTargetPtr;
    //////////////////////////////////////////////////////////////////////////
}
