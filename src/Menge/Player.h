#	pragma once

#   include "Interface/PlayerInterface.h"
#   include "Interface/NotificationServiceInterface.h"
#   include "Interface/MousePickerSystemInterface.h"
#   include "Interface/GlobalHandleSystemInterface.h"
#   include "Interface/ScheduleManagerInterface.h"

#   include "Core/ServiceBase.h"
#	include "Core/ConstString.h"
#	include "Core/Resolution.h"

#   include "Factory/Factory.h"

#	include "Math/vec3.h"

#	include "pybind/types.hpp"

namespace Menge
{	
    class Game;
	class Scene;
	class Arrow;
	class TextField;

	class Node;
	class Join;

	class RenderViewport;
	class RenderCameraOrthogonal;

	class MousePickerSystemInterface;
	class GlobalHandleSystem;

	class EventManager;

	class Player
		: public ServiceBase<PlayerServiceInterface>
	{
	public:
		Player();
		~Player();

	public:
        bool _initialize() override;
		void _finalize() override;

	public:
		bool setCurrentScene( Scene * _scene, bool _destroyOld, const SceneChangeCallbackInterfacePtr & _cb ) override;
		bool restartCurrentScene( const SceneChangeCallbackInterfacePtr & _cb ) override;
		bool removeCurrentScene( const SceneChangeCallbackInterfacePtr & _cb ) override;
		void destroyCurrentScene() override;

		Scene * getCurrentScene() override;

	public:
		bool createGlobalScene() override;
		void removeGlobalScene() override;
		Scene * getGlobalScene() override;
            
	public:
		void setArrow( Arrow * _arrow ) override;
		Arrow * getArrow() const override;

	public:
		void calcGlobalMouseWorldPosition( const mt::vec2f & _screenPoint, mt::vec2f & _worldPoint ) override;
		void calcGlobalMouseWorldDelta( const mt::vec2f & _screenPoint, const mt::vec2f & _screenDeltha, mt::vec2f & _worldDeltha ) override;

	public:
		ScheduleManagerInterfacePtr createSchedulerManager() override;
		bool destroySchedulerManager( const ScheduleManagerInterfacePtr & _scheduler ) override;

    public:
        MousePickerSystemInterface * getMousePickerSystem() const override;
        GlobalHandleSystemInterface * getGlobalHandleSystem() const override;

    public:
        const ScheduleManagerInterfacePtr & getScheduleManager() const override;
        const ScheduleManagerInterfacePtr & getScheduleManagerGlobal() const override;

	public:
		Affectorable * getAffectorable() const override;
		Affectorable * getAffectorableGlobal() const override;

	public:
		void setRenderCamera( RenderCameraInterface * _camera) override;
		const RenderCameraInterface * getRenderCamera() const override;

	public:
		void setRenderViewport( RenderViewportInterface * _viewport ) override;
		const RenderViewportInterface * getRenderViewport() const override;

	public:
		void setRenderClipplane( RenderClipplaneInterface * _clipplane ) override;
		const RenderClipplaneInterface * getRenderClipplane() const override;

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
		Scene * m_scene;
		Arrow * m_arrow;

		Scene * m_globalScene;

		RenderCameraOrthogonal * m_camera2D;
		RenderViewport * m_viewport2D;

		RenderCameraOrthogonal * m_arrowCamera2D;
		
		RenderViewportInterface * m_renderViewport;
		RenderCameraInterface * m_renderCamera;		
		RenderClipplaneInterface * m_renderClipplane;
		RenderTargetInterface * m_renderTarget;

        MousePickerSystemInterface * m_mousePickerSystem;
        GlobalHandleSystemInterface * m_globalHandleSystem;

		ScheduleManagerInterfacePtr m_scheduleManager;
		ScheduleManagerInterfacePtr m_scheduleManagerGlobal;

        FactoryPtr m_factoryScheduleManager;

		typedef stdex::vector<ScheduleManagerInterfacePtr> TVectorUserScheduler;
		TVectorUserScheduler m_schedulers;               

		Affectorable * m_affectorable;
		Affectorable * m_affectorableGlobal;

		bool m_arrowHided;
		bool m_focus;
		
		Scene * m_switchSceneTo;

		//bool m_switchScene2;
		//bool m_switchBlock;
		//bool m_switchScene;
		//bool m_restartScene;
		//bool m_removeScene;

		//bool m_destroyOldScene;
				
		//SceneChangeCallbackInterfacePtr m_changeSceneCb;
		//SceneChangeCallbackInterfacePtr m_removeSceneCb;

		uint32_t m_fps;

//#	ifndef MENGINE_MASTER_RELEASE
	public:
		void toggleDebugText() override;

	protected:
		uint32_t m_showDebugText;
		TextField * m_debugText;
		RenderCameraOrthogonal * m_debugCamera2D;
//#	endif
	};
}
