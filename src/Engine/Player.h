#pragma once

#include "Interface/PlayerInterface.h"
#include "Interface/NotificationServiceInterface.h"
#include "Interface/MousePickerSystemInterface.h"
#include "Interface/GlobalHandleSystemInterface.h"
#include "Interface/ScheduleManagerInterface.h"

#include "Core/ServiceBase.h"
#include "Core/ConstString.h"
#include "Core/Resolution.h"

#include "Factory/Factory.h"

#include "math/vec3.h"

#include "pybind/types.hpp"

namespace Mengine
{	
    typedef IntrusivePtr<class Node> NodePtr;
    typedef IntrusivePtr<class Game> GamePtr;
	typedef IntrusivePtr<class Scene> ScenePtr;
	typedef IntrusivePtr<class Arrow> ArrowPtr;
	typedef IntrusivePtr<class TextField> TextFieldPtr;    
    typedef IntrusivePtr<class RenderViewport> RenderViewportPtr;
    typedef IntrusivePtr<class RenderCameraOrthogonal> RenderCameraOrthogonalPtr;
    typedef IntrusivePtr<class RenderTargetInterface> RenderTargetInterfacePtr;
	typedef IntrusivePtr<class MousePickerSystemInterface> MousePickerSystemInterfacePtr;
	typedef IntrusivePtr<class GlobalHandleSystem> GlobalHandleSystemPtr;
    typedef IntrusiveDerivedPtr<class GlobalAffectorable, class Affectorable> GlobalAffectorablePtr;
    
	class Player
		: public ServiceBase<PlayerServiceInterface>
	{
	public:
		Player();
		~Player() override;

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
		void calcGlobalMouseWorldDelta( const mt::vec2f & _screenPoint, const mt::vec2f & _screenDeltha, mt::vec2f & _worldDeltha ) override;

	public:
		ScheduleManagerInterfacePtr createSchedulerManager() override;
		bool destroySchedulerManager( const ScheduleManagerInterfacePtr & _scheduler ) override;

    public:
        const MousePickerSystemInterfacePtr & getMousePickerSystem() const override;
        const GlobalHandleSystemInterfacePtr & getGlobalHandleSystem() const override;

    public:
        const ScheduleManagerInterfacePtr & getScheduleManager() const override;
        const ScheduleManagerInterfacePtr & getScheduleManagerGlobal() const override;

	public:
		const AffectorablePtr & getAffectorable() const override;
        const AffectorablePtr & getAffectorableGlobal() const override;

	public:
		void setRenderCamera( const RenderCameraInterfacePtr & _camera) override;
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
		void tick( float _time, float _timing ) override;
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

        MousePickerSystemInterfacePtr m_mousePickerSystem;
        GlobalHandleSystemInterfacePtr m_globalHandleSystem;

		ScheduleManagerInterfacePtr m_scheduleManager;
		ScheduleManagerInterfacePtr m_scheduleManagerGlobal;

        FactoryPtr m_factoryScheduleManager;

		typedef stdex::vector<ScheduleManagerInterfacePtr> TVectorUserScheduler;
		TVectorUserScheduler m_schedulers;               

        GlobalAffectorablePtr m_affectorable;
        GlobalAffectorablePtr m_affectorableGlobal;



		bool m_arrowHided;
		bool m_focus;
		
		ScenePtr m_switchSceneTo;

		uint32_t m_fps;

	public:
		void toggleDebugText() override;

	protected:
		uint32_t m_showDebugText;
		TextFieldPtr m_debugText;
		RenderCameraOrthogonalPtr m_debugCamera2D;
	};
}
