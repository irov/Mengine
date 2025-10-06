#pragma once

#include "Interface/PlayerServiceInterface.h"
#include "Interface/PickerServiceInterface.h"
#include "Interface/GlobalInputHandlerInterface.h"
#include "Interface/SchedulerInterface.h"
#include "Interface/LayoutInterface.h"
#include "Interface/RenderTargetInterface.h"
#include "Interface/RenderPipelineInterface.h"
#include "Interface/FactoryInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/ConstString.h"
#include "Kernel/Resolution.h"
#include "Kernel/Affectorable.h"
#include "Kernel/RenderResolution.h"
#include "Kernel/RenderViewportDefault.h"
#include "Kernel/RenderCameraOrthogonalDefault.h"
#include "Kernel/RenderRoot.h"

namespace Mengine
{
    class PlayerService
        : public ServiceBase<PlayerServiceInterface>
    {
    public:
        PlayerService();
        ~PlayerService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;
        void _stopService() override;

    public:
        void setArrow() override;

    public:
        void calcGlobalMouseWorldPosition( const mt::vec2f & _screenPoint, mt::vec2f * const _worldPoint ) override;
        void calcGlobalMouseWorldDelta( const mt::vec2f & _screenDelta, mt::vec2f * const _worldDelta ) override;

    public:
        SchedulerInterfacePtr createScheduler( const DocumentInterfacePtr & _doc ) override;
        void destroyScheduler( const SchedulerInterfacePtr & _scheduler ) override;

    public:
        LayoutInterfacePtr createLayout( const DocumentInterfacePtr & _doc ) override;
        void destroyLayout( const LayoutInterfacePtr & _layout ) override;

    public:
        const GlobalInputHandlerInterfacePtr & getGlobalInputHandler() const override;

    public:
        const SchedulerInterfacePtr & getLocalScheduler() const override;
        const SchedulerInterfacePtr & getGlobalScheduler() const override;

    public:
        const RandomizerInterfacePtr & getRandomizer() const override;

    public:
        const AffectorHubInterfacePtr & getAffectorHub() const override;
        const AffectorHubInterfacePtr & getGlobalAffectorHub() const override;

    public:
        const RenderCameraInterfacePtr & getDefaultSceneRenderCamera2D() const override;
        const RenderViewportInterfacePtr & getDefaultRenderViewport2D() const override;
        const RenderCameraInterfacePtr & getDefaultArrowRenderCamera2D() const override;

    public:
        void setRenderResolution( const RenderResolutionInterfacePtr & _resolution ) override;
        const RenderResolutionInterfacePtr & getRenderResolution() const override;

    public:
        void setRenderViewport( const RenderViewportInterfacePtr & _viewport ) override;
        const RenderViewportInterfacePtr & getRenderViewport() const override;

    public:
        void setRenderCamera( const RenderCameraInterfacePtr & _camera ) override;
        const RenderCameraInterfacePtr & getRenderCamera() const override;

    public:
        void setRenderTransformation( const RenderTransformationInterfacePtr & _transformation ) override;
        const RenderTransformationInterfacePtr & getRenderTransformation() const override;

    public:
        void setRenderScissor( const RenderScissorInterfacePtr & _scissor ) override;
        const RenderScissorInterfacePtr & getRenderScissor() const override;

    public:
        void setRenderTarget( const RenderTargetInterfacePtr & _target ) override;
        const RenderTargetInterfacePtr & getRenderTarget() const override;

    public:
        const RenderContext * getRenderContext() const override;

    public:
        const RenderInterface * getRenderExtra() const override;

    public:
        void initializeRenderResources() override;
        void finalizeRenderResources() override;

    public:
        void update() override;

    public:
        void render( const RenderPipelineInterfacePtr & _renderPipeline ) override;

    public:
        bool handleKeyEvent( const InputKeyEvent & _event ) override;
        bool handleTextEvent( const InputTextEvent & _event ) override;

    public:
        bool handleAccelerometerEvent( const InputAccelerometerEvent & _event ) override;

    public:
        bool handleMouseButtonEvent( const InputMouseButtonEvent & _event ) override;
        bool handleMouseButtonEventBegin( const InputMouseButtonEvent & _event ) override;
        bool handleMouseButtonEventEnd( const InputMouseButtonEvent & _event ) override;
        bool handleMouseMove( const InputMouseMoveEvent & _event ) override;
        bool handleMouseWheel( const InputMouseWheelEvent & _event ) override;

    public:
        bool handleMouseEnter( const InputMouseEnterEvent & _event ) override;
        void handleMouseLeave( const InputMouseLeaveEvent & _event ) override;

    public:
        void onFocus( bool _focus ) override;

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
        RenderResolutionPtr m_defaultResolution;
        RenderCameraOrthogonalDefaultPtr m_defaultCamera2D;
        RenderViewportDefaultPtr m_defaultViewport2D;
        RenderCameraOrthogonalDefaultPtr m_defaultArrowCamera2D;

        RenderResolutionInterfacePtr m_renderResolution;
        RenderViewportInterfacePtr m_renderViewport;
        RenderCameraInterfacePtr m_renderCamera;
        RenderTransformationInterfacePtr m_renderTransformation;
        RenderScissorInterfacePtr m_renderScissor;
        RenderTargetInterfacePtr m_renderTarget;

        RenderContext m_renderContext;

        RenderRootPtr m_renderExtra;

        GlobalInputHandlerInterfacePtr m_globalInputHandler;

        SchedulerInterfacePtr m_localScheduler;
        SchedulerInterfacePtr m_globalScheduler;

        RandomizerInterfacePtr m_randomizer;

        FactoryInterfacePtr m_factoryScheduler;
        FactoryInterfacePtr m_factoryLayout;

        AffectorablePtr m_affectorable;
        AffectorablePtr m_affectorableGlobal;

        bool m_focus;
    };
}
