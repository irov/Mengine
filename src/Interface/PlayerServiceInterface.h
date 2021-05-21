#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/SchedulerInterface.h"
#include "Interface/PickerServiceInterface.h"
#include "Interface/GlobalInputHandlerInterface.h"
#include "Interface/UpdationInterface.h"
#include "Interface/RandomizerInterface.h"

#include "Kernel/RenderCameraOrthogonal.h"
#include "Kernel/RenderViewport.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class Arrow, class Node> ArrowPtr;
    typedef IntrusivePtr<class Affectorable> AffectorablePtr;
    typedef IntrusivePtr<class RenderViewportInterface> RenderViewportInterfacePtr;
    typedef IntrusivePtr<class RenderCameraInterface> RenderCameraInterfacePtr;
    typedef IntrusivePtr<class RenderScissorInterface> RenderScissorInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class PlayerServiceInterface
        : public ServiceInterface
        , public InputHandlerInterface
    {
        SERVICE_DECLARE( "PlayerService" )

    public:
        virtual void onFocus( bool _focus ) = 0;

        virtual void onFullscreen( const Resolution & _resolution, bool _fullscreen ) = 0;
        virtual void onFixedContentResolution( const Resolution & _resolution, bool _fixed ) = 0;
        virtual void onFixedDisplayResolution( const Resolution & _resolution, bool _fixed ) = 0;

    public:
        virtual bool update() = 0;
        virtual void render( const RenderPipelineInterfacePtr & _renderPipeline ) = 0;

    public:
        virtual void initializeRenderResources() = 0;
        virtual void finalizeRenderResources() = 0;

    public:
        virtual void setArrow( const ArrowPtr & _arrow ) = 0;
        virtual const ArrowPtr & getArrow() const = 0;

    public:
        virtual void calcGlobalMouseWorldPosition( const mt::vec2f & _screenPoint, mt::vec2f * const _worldPoint ) = 0;
        virtual void calcGlobalMouseWorldDelta( const mt::vec2f & _screenDeltha, mt::vec2f * const _worldDeltha ) = 0;

    public:
        virtual SchedulerInterfacePtr createScheduler( const ConstString & _name, const DocumentPtr & _doc ) = 0;
        virtual bool destroyScheduler( const SchedulerInterfacePtr & _scheduler ) = 0;

    public:
        virtual const RenderCameraOrthogonalPtr & getDefaultCamera2D() const = 0;
        virtual const RenderViewportPtr & getDefaultViewport2D() const = 0;
        virtual const RenderCameraOrthogonalPtr & getDefaultArrowCamera2D() const = 0;

    public:
        virtual void setRenderViewport( const RenderViewportInterfacePtr & _renderViewport ) = 0;
        virtual const RenderViewportInterfacePtr & getRenderViewport() const = 0;

    public:
        virtual void setRenderCamera( const RenderCameraInterfacePtr & _camera ) = 0;
        virtual const RenderCameraInterfacePtr & getRenderCamera() const = 0;

    public:
        virtual void setRenderTransformation( const RenderTransformationInterfacePtr & _transformation ) = 0;
        virtual const RenderTransformationInterfacePtr & getRenderTransformation() const = 0;

    public:
        virtual void setRenderScissor( const RenderScissorInterfacePtr & _scissor ) = 0;
        virtual const RenderScissorInterfacePtr & getRenderScissor() const = 0;

    public:
        virtual void setRenderTarget( const RenderTargetInterfacePtr & _target ) = 0;
        virtual const RenderTargetInterfacePtr & getRenderTarget() const = 0;

    public:
        virtual const RenderContext * getRenderContext() const = 0;

    public:
        virtual const GlobalInputHandlerInterfacePtr & getGlobalInputHandler() const = 0;

    public:
        virtual const SchedulerInterfacePtr & getLocalScheduler() const = 0;
        virtual const SchedulerInterfacePtr & getGlobalScheduler() const = 0;

    public:
        virtual const RandomizerInterfacePtr & getRandomizer() const = 0;

    public:
        virtual const AffectorHubInterfacePtr & getAffectorHub() const = 0;
        virtual const AffectorHubInterfacePtr & getGlobalAffectorHub() const = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define PLAYER_SERVICE()\
    ((Mengine::PlayerServiceInterface*)SERVICE_GET(Mengine::PlayerServiceInterface))
//////////////////////////////////////////////////////////////////////////