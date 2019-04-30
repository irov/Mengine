#pragma once

#include "Interface/PlayerServiceInterface.h"
#include "Interface/NotificationServiceInterface.h"
#include "Interface/PickerServiceInterface.h"
#include "Interface/GlobalInputHandlerInterface.h"
#include "Interface/SchedulerInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/ConstString.h"
#include "Kernel/Resolution.h"
#include "Kernel/Observable.h"
#include "Kernel/Factory.h"
#include "Kernel/RenderCameraOrthogonal.h"
#include "Kernel/RenderViewport.h"

#include "math/vec3.h"

#include "pybind/types.hpp"

namespace Mengine
{
    class PlayerService
        : public ServiceBase<PlayerServiceInterface>
        , public Observable
    {
    public:
        PlayerService();
        ~PlayerService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        void setArrow( const ArrowPtr & _arrow ) override;
        const ArrowPtr & getArrow() const override;

    public:
        void calcGlobalMouseWorldPosition( const mt::vec2f & _screenPoint, mt::vec2f * _worldPoint ) override;
        void calcGlobalMouseWorldDelta( const mt::vec2f & _screenDeltha, mt::vec2f * _worldDeltha ) override;

    public:
        SchedulerInterfacePtr createSchedulerManager( const ConstString & _name ) override;
        bool destroySchedulerManager( const SchedulerInterfacePtr & _scheduler ) override;

    public:
        const GlobalInputHandlerInterfacePtr & getGlobalInputHandler() const override;

    public:
        const SchedulerInterfacePtr & getScheduler() const override;
        const SchedulerInterfacePtr & getGlobalScheduler() const override;

    public:
        const RandomizerInterfacePtr & getRandomizer() const override;

    public:
        const AffectorablePtr & getAffectorable() const override;
        const AffectorablePtr & getGlobalAffectorable() const override;

    public:
        void setRenderCamera( const RenderCameraInterfacePtr & _camera ) override;
        const RenderCameraInterfacePtr & getRenderCamera() const override;

    public:
        void setRenderViewport( const RenderViewportInterfacePtr & _viewport ) override;
        const RenderViewportInterfacePtr & getRenderViewport() const override;

    public:
        void setRenderScissor( const RenderScissorInterfacePtr & _scissor ) override;
        const RenderScissorInterfacePtr & getRenderScissor() const override;

    public:
        void initializeRenderResources() override;
        void finalizeRenderResources() override;

    public:
        void tick( const UpdateContext * _context ) override;
        bool update() override;

    public:
        void render() override;

    public:
        bool handleKeyEvent( const InputKeyEvent & _event ) override;
        bool handleTextEvent( const InputTextEvent & _event ) override;

    public:
        bool handleMouseButtonEvent( const InputMouseButtonEvent & _event ) override;
        bool handleMouseButtonEventBegin( const InputMouseButtonEvent & _event ) override;
        bool handleMouseButtonEventEnd( const InputMouseButtonEvent & _event ) override;
        bool handleMouseMove( const InputMouseMoveEvent & _event ) override;
        bool handleMouseWheel( const InputMouseWheelEvent & _event ) override;

    public:
        void onFocus( bool _focus ) override;

        void onAppMouseLeave( const InputMousePositionEvent & _event ) override;
        void onAppMouseEnter( const InputMousePositionEvent & _event ) override;
        void onAppMousePosition( const InputMousePositionEvent & _event ) override;

        void onFullscreen( const Resolution & _resolution, bool _fullscreen ) override;
        void onFixedContentResolution( const Resolution & _resolution, bool _fixed ) override;
        void onFixedDisplayResolution( const Resolution & _resolution, bool _fixed ) override;

    protected:
        void notifyChangeScenePrepareDestroy( const ScenePtr & _oldScene, const ScenePtr & _newScene );
        void notifyChangeSceneDestroy( const ScenePtr & _scene );
        void notifyChangeScenePrepareInitialize( const ScenePtr & _scene );
        void notifyChangeSceneEnable( const ScenePtr & _scene );
        void notifyChangeScenePrepareComplete( const ScenePtr & _scene );
        void notifyRestartScenePrepareDisable( const ScenePtr & _scene );
        void notifyRestartSceneDisable( const ScenePtr & _scene );
        void notifyRestartSceneEnable( const ScenePtr & _scene );
        void notifyRemoveScenePrepareDestroy( const ScenePtr & _scene );
        void notifyRemoveSceneDestroy();

    protected:
        ArrowPtr m_arrow;

        RenderCameraOrthogonalPtr m_camera2D;
        RenderViewportPtr m_viewport2D;
        RenderCameraOrthogonalPtr m_arrowCamera2D;

        RenderViewportInterfacePtr m_renderViewport;
        RenderCameraInterfacePtr m_renderCamera;
        RenderScissorInterfacePtr m_renderScissor;
        RenderTargetInterfacePtr m_renderTarget;

        GlobalInputHandlerInterfacePtr m_globalInputHandler;

        SchedulerInterfacePtr m_scheduleManager;
        SchedulerInterfacePtr m_scheduleManagerGlobal;

        RandomizerInterfacePtr m_randomizer;

        FactoryPtr m_factoryScheduleManager;

        typedef Vector<SchedulerInterfacePtr> VectorUserScheduler;
        VectorUserScheduler m_schedulers;

        AffectorablePtr m_affectorable;
        AffectorablePtr m_affectorableGlobal;

        bool m_focus;
    };
}
