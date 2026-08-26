#pragma once

#include "OpenGLRenderImageBase.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class OpenGLRenderImageArray
        : public OpenGLRenderImageBase
    {
        DECLARE_FACTORABLE( RenderImageInterface );

    public:
        OpenGLRenderImageArray();
        ~OpenGLRenderImageArray() override;

    public:
        GLenum getTextureTarget() const override;

    protected:
        bool _create() override;
        bool _unlock( const Rect & _lockedRect, const void * _buffer, uint32_t _layer, uint32_t _level ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<OpenGLRenderImageArray, RenderImageInterface> OpenGLRenderImageArrayPtr;
    //////////////////////////////////////////////////////////////////////////
}
