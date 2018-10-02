#pragma once

#include "Interface/RenderViewportInterface.h"
#include "Interface/RenderCameraInterface.h"
#include "Interface/RenderTransformationInterface.h"
#include "Interface/RenderScissorInterface.h"
#include "Interface/RenderTargetInterface.h"

#include "Kernel/Mixin.h"
#include "Kernel/Colorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    struct RenderContext
    {
        RenderViewportInterfacePtr viewport;
        RenderCameraInterfacePtr camera;
        RenderTransformationInterfacePtr transformation;
        RenderScissorInterfacePtr scissor;
        RenderTargetInterfacePtr target;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class RenderInterface> RenderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class RenderInterface
        : public Colorable
    {
    public:
        virtual void setRelationRender( const RenderInterfacePtr & _relationRender ) = 0;

    public:
        virtual void setHide( bool _hide ) = 0;
        virtual bool isHide() const = 0;

    public:
        virtual void setLocalHide( bool _localHide ) = 0;
        virtual bool isLocalHide() const = 0;

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
        virtual const RenderInterfacePtr & makeTargetRender( const RenderContext * _state ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<RenderInterface> RenderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}