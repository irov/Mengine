#pragma once

#include "Interface/RenderViewportInterface.h"
#include "Interface/RenderCameraInterface.h"
#include "Interface/RenderTransformationInterface.h"
#include "Interface/RenderScissorInterface.h"
#include "Interface/RenderTargetInterface.h"

#include "Kernel/Mixin.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    struct RenderContext
    {
        RenderContext()
            :debugMask( 0 )
        {}

        RenderViewportInterfacePtr viewport;
        RenderCameraInterfacePtr camera;
        RenderTransformationInterfacePtr transformation;
        RenderScissorInterfacePtr scissor;
        RenderTargetInterfacePtr target;

        uint32_t debugMask;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class RenderInterface> RenderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class RenderInterface
        : public Mixin
    {
    public:
        virtual void setRenderViewport( const RenderViewportInterfacePtr & _viewport ) = 0;
        virtual const RenderViewportInterfacePtr & getRenderViewport() const = 0;

    public:
        virtual void setRenderCamera( const RenderCameraInterfacePtr & _camera ) = 0;
        virtual const RenderCameraInterfacePtr & getRenderCamera() const = 0;

    public:
        virtual void setRenderScissor( const RenderScissorInterfacePtr & _scissor ) = 0;
        virtual const RenderScissorInterfacePtr & getRenderScissor() const = 0;

    public:
        virtual void setRenderTarget( const RenderTargetInterfacePtr & _target ) = 0;
        virtual const RenderTargetInterfacePtr & getRenderTarget() const = 0;

    public:
        virtual void setExternalRender( bool _externalRender ) = 0;
        virtual bool isExternalRender() const = 0;

    public:
        virtual void render( const RenderContext * _state ) = 0;

    public:
        virtual const RenderInterfacePtr & renderTarget( const RenderContext * _state ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<RenderInterface> RenderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}