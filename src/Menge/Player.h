#	pragma once

#   include "Interface/PlayerInterface.h"

#	include "Core/ConstString.h"
#	include "Core/Resolution.h"

#   include "Factory/FactoryStore.h"

#	include "Math/vec3.h"

#	include "pybind/types.hpp"

#   include "MousePickerSystem.h"
#   include "GlobalHandleSystem.h"
#   include "Kernel/ScheduleManager.h"
#   include "Kernel/TimingManager.h"


namespace Menge
{	
    class Game;
	class Scene;
	class Arrow;
	class TextField;

	class Node;
	class Join;

	class RenderViewport;
	class Camera2D;

	class MousePickerSystemInterface;
	class GlobalHandleSystem;

	class ScheduleManagerInterface;
	class TimingManagerInterface;

	class EventManager;

	class Player
        : public PlayerServiceInterface		
	{
	public:
		Player();
		~Player();

    public:
        void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
        ServiceProviderInterface * getServiceProvider() const override;

    public:
        bool initialize() override;
		void finalize() override;

	public:
		bool setCurrentScene( Scene * _scene, bool _destroyOld, const pybind::object & _cb ) override;
		bool restartCurrentScene( const pybind::object & _cb ) override;
		bool removeCurrentScene( const pybind::object & _cb ) override;

		Scene * getCurrentScene() override;

		bool isChangedScene() const override;

	public:
		void updateChangeScene() override;

    protected:        
		void updateSwitchScene_();
		void updateRemoveScene_();
		void updateRestartScene_();

	public:
		void setArrow( Arrow * _arrow ) override;
		Arrow * getArrow() const override;

	public:
		void calcGlobalMouseWorldPosition( const mt::vec2f & _screenPoint, mt::vec2f & _worldPoint ) override;
		void calcGlobalMouseWorldDelta( const mt::vec2f & _screenPoint, const mt::vec2f & _screenDeltha, mt::vec2f & _worldDeltha ) override;

	public:
		ScheduleManagerInterface * createSchedulerManager() override;
		bool destroySchedulerManager( ScheduleManagerInterface * _scheduler ) override;

    public:
        MousePickerSystemInterface * getMousePickerSystem() const override;
        GlobalHandleSystemInterface * getGlobalHandleSystem() const override;

    public:
		ScheduleManagerInterface * getScheduleManager() const override;
		ScheduleManagerInterface * getScheduleManagerGlobal() const override;

	public:
		TimingManagerInterface * createTimingManager() override;
		bool destroyTimingManager( TimingManagerInterface * _timing ) override;

    public:
		TimingManagerInterface * getTimingManager() const override;
        TimingManagerInterface * getTimingManagerGlobal() const override;

	public:
		Affectorable * getAffectorable() const override;
		Affectorable * getAffectorableGlobal() const override;

	public:
		void setRenderCamera( RenderCameraInterface * _camera) override;
		const RenderCameraInterface * getRenderCamera() const override;

	public:
		void setRenderViewport( RenderViewportInterface * _renderViewport ) override;
		const RenderViewportInterface * getRenderViewport() const override;

    public:
		void initializeRenderResources() override;
		void finalizeRenderResources() override;

    public:
		void tick( float _timing );
		bool update() override;


	public:
		float getTime() const override;

	public:
		void render() override;
		
	public:
		bool handleKeyEvent( const InputKeyEvent & _event ) override;

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
	
	public:
		Join * addJoin( Node * _left, Node * _right, const mt::vec2f & _offset ) override;
		void removeJoin( Join * _join ) override;
		bool isJoin( Node * _left, Node * _right ) const override;

		void getJoins( Node * _node, TVectorNode & _joins ) const override;

	protected:
		void updateJoins_();

	protected:
		typedef stdex::vector<Join *> TVectorJoins;
		TVectorJoins m_joins;

	protected:
		void renderArrow_( unsigned int _debugMask );

	private:
        ServiceProviderInterface * m_serviceProvider;

		Scene * m_scene;
		Arrow * m_arrow;
		
		Camera2D * m_camera2D;
		RenderViewport * m_viewport2D;

		Camera2D * m_arrowCamera2D;
		
		RenderCameraInterface * m_renderCamera;
		RenderViewportInterface * m_renderViewport;

        MousePickerSystem * m_mousePickerSystem;
        GlobalHandleSystem * m_globalHandleSystem;

		ScheduleManagerInterface * m_scheduleManager;
		ScheduleManagerInterface * m_scheduleManagerGlobal;

		typedef stdex::vector<ScheduleManagerInterface *> TVectorUserScheduler;
		TVectorUserScheduler m_schedulers;

        FactoryDefaultStore<ScheduleManager> m_factoryScheduleManager;

		TimingManagerInterface * m_timingManager;
        TimingManagerInterface * m_timingManagerGlobal;

		typedef stdex::vector<TimingManagerInterface *> TVectorUserTiming;
		TVectorUserTiming m_timingers;

        FactoryDefaultStore<TimingManager> m_factoryTimingManager;

		Affectorable * m_affectorable;
		Affectorable * m_affectorableGlobal;

		bool m_arrowHided;
		
		Scene * m_switchSceneTo;

		bool m_switchScene;		
		bool m_restartScene;
		bool m_removeScene;

		bool m_destroyOldScene;
				
		pybind::object m_changeSceneCb;
		pybind::object m_removeSceneCb;

		float m_time;

		uint32_t m_fps;

//#	ifndef MENGINE_MASTER_RELEASE
	public:
		void toggleDebugText() override;

	protected:
		uint32_t m_showDebugText;
		TextField * m_debugText;
		Camera2D * m_debugCamera2D;
//#	endif
	};
}
