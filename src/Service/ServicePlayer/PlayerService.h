#pragma once

#include "Interface/PlayerInterface.h"
#include "Interface/NotificationServiceInterface.h"
#include "Interface/PickerServiceInterface.h"
#include "Interface/GlobalHandleSystemInterface.h"
#include "Interface/SchedulerInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/ConstString.h"
#include "Kernel/Resolution.h"
#include "Kernel/Factory.h"

#include "math/vec3.h"

#include "pybind/types.hpp"

namespace Mengine
{
    typedef IntrusivePtr<class Scene, Node> ScenePtr;
    typedef IntrusivePtr<class Arrow> ArrowPtr;
    typedef IntrusivePtr<class TextField> TextFieldPtr;
    typedef IntrusivePtr<class RenderViewport> RenderViewportPtr;
    typedef IntrusivePtr<class RenderCameraOrthogonal> RenderCameraOrthogonalPtr;
    typedef IntrusivePtr<class RenderTargetInterface> RenderTargetInterfacePtr;
    typedef IntrusivePtr<class GlobalHandleSystem> GlobalHandleSystemPtr;
    typedef IntrusivePtr<class GlobalAffectorable, class Affectorable> GlobalAffectorablePtr;

    class PlayerService
        : public ServiceBase<PlayerServiceInterface>
    {
    public:
        PlayerService();
        ~PlayerService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        bool setCurrentScene( const ScenePtr & _scene, bool _destroyOld, const SceneChangeCallbackInterfacePtr & _cb ) override;
        bool restartCurrentScene( const SceneChangeCallbackInterfacePtr & _cb ) override;
        bool removeCurrentScene( const SceneChangeCallbackInterfacePtr & _cb ) override;
        void destroyCurrentScene() override;

        const ScenePtr & getCurrentScene() override;

    public:
        bool createGlobalScene() override;
        void removeGlobalScene() override;
        const ScenePtr & getGlobalScene() override;

    public:
        void setArrow( const ArrowPtr & _arrow ) override;
        const ArrowPtr & getArrow() const override;

    public:
        void calcGlobalMouseWorldPosition( const mt::vec2f & _screenPoint, mt::vec2f & _worldPoint ) override;
        void calcGlobalMouseWorldDelta( const mt::vec2f & _screenDeltha, mt::vec2f & _worldDeltha ) override;

    public:
        SchedulerInterfacePtr createSchedulerManager( const ConstString & _name ) override;
        bool destroySchedulerManager( const SchedulerInterfacePtr & _scheduler ) override;

    public:
        const GlobalHandleSystemInterfacePtr & getGlobalHandleSystem() const override;

    public:
        const SchedulerInterfacePtr & getScheduleManager() const override;
        const SchedulerInterfacePtr & getGlobalScheduleManager() const override;

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
        void renderArrow_( uint32_t _debugMask );

    private:
        ScenePtr m_scene;
        ArrowPtr m_arrow;

        ScenePtr m_globalScene;

        RenderCameraOrthogonalPtr m_camera2D;
        RenderViewportPtr m_viewport2D;

        RenderCameraOrthogonalPtr m_arrowCamera2D;

        RenderViewportInterfacePtr m_renderViewport;
        RenderCameraInterfacePtr m_renderCamera;
        RenderScissorInterfacePtr m_renderScissor;
        RenderTargetInterfacePtr m_renderTarget;

        PickerServiceInterfacePtr m_pickerService;
        GlobalHandleSystemInterfacePtr m_globalHandleSystem;

        SchedulerInterfacePtr m_scheduleManager;
        SchedulerInterfacePtr m_scheduleManagerGlobal;

        FactoryPtr m_factoryScheduleManager;

        typedef Vector<SchedulerInterfacePtr> VectorUserScheduler;
        VectorUserScheduler m_schedulers;

        GlobalAffectorablePtr m_affectorable;
        GlobalAffectorablePtr m_affectorableGlobal;

        bool m_arrowHided;
        bool m_focus;

        ScenePtr m_switchSceneTo;

        uint32_t m_fps;

    protected:        
        RenderCameraOrthogonalPtr m_debugCamera2D;
    };
}
