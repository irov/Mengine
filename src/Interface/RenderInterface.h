#pragma once

#include "Interface/Interface.h"
#include "Interface/RenderPipelineInterface.h"

#include "Kernel/Colorable.h"
#include "Kernel/BoundingBox.h"
#include "Kernel/RenderContext.h"

#include "Config/Lambda.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Renderable;
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class RenderInterface> RenderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class RenderInterface
        : public Interface
        , public Colorable
        , public BoundingBox
    {
    public:
        virtual Renderable * getRenderable() = 0;

    public:
        virtual void setRelationRender( RenderInterface * _relationRender ) = 0;
        virtual void setRelationRenderFront( RenderInterface * _relationRender ) = 0;
        virtual void removeRelationRender() = 0;
        virtual RenderInterface * getRelationRender() const = 0;

    public:
        virtual void setExtraRelationRender( RenderInterface * _relationRender ) = 0;
        virtual void removeExtraRelationRender() = 0;
        virtual RenderInterface * getExtraRelationRender() const = 0;

    public:
        virtual RenderInterface * getTotalRelationRender() const = 0;

    public:
        virtual bool emptyRenderChildren() const = 0;
        virtual void clearRenderChildren() = 0;

    public:
        typedef Lambda<void( RenderInterface * )> LambdaRender;
        virtual void foreachRenderChildren( const LambdaRender & _lambda ) = 0;
        virtual void foreachRenderChildrenEnabled( const LambdaRender & _lambda ) = 0;

    public:
        virtual void setRenderEnable( bool _enable ) = 0;
        virtual bool isRenderEnable() const = 0;

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
        virtual void setRenderTransformation( const RenderTransformationInterfacePtr & _renderTransformation ) = 0;
        virtual const RenderTransformationInterfacePtr & getRenderTransformation() const = 0;

    public:
        virtual void setRenderScissor( const RenderScissorInterfacePtr & _scissor ) = 0;
        virtual const RenderScissorInterfacePtr & getRenderScissor() const = 0;

    public:
        virtual void setRenderTarget( const RenderTargetInterfacePtr & _target ) = 0;
        virtual const RenderTargetInterfacePtr & getRenderTarget() const = 0;

    public:
        virtual void setZGroup( ZGroupType _zGroup ) = 0;
        virtual ZGroupType getZGroup() const = 0;

    public:
        virtual void setZIndex( ZIndexType _zIndex ) = 0;
        virtual ZIndexType getZIndex() const = 0;

    public:
        virtual void mergeRenderContext( const RenderContext * _in, RenderContext * const _out ) const = 0;
        virtual void makeRenderContext( RenderContext * const _context ) const = 0;

    public:
        virtual void setExternalRender( bool _externalRender ) = 0;
        virtual bool isExternalRender() const = 0;

    public:
        virtual void render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const = 0;

    public:
        virtual void renderWithChildren( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context, bool _external ) const = 0;

    public:
        virtual const RenderInterfacePtr & makeTargetRender( const RenderContext * _context ) const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<RenderInterface> RenderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}