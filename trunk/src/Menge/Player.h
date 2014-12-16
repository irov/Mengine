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
	class HotSpot;
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
		, public InputSystemHandler
	{
	public:
		Player( Game * _game );
		~Player();

    public:
        void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
        ServiceProviderInterface * getServiceProvider() const override;

    public:
        bool initialize( const Resolution & _contentResolution, const Resolution & _currentResolution );
        void finalize();

	public:
		bool setCurrentScene( const ConstString& _scene, bool _destroyOld, bool _destroyAfterSwitch, PyObject* _cb );
		bool removeCurrentScene( PyObject * _cb );

		Scene * getCurrentScene() override;

		bool isChangedScene() const;

	public:
		void updateChangeScene();

    protected:        
		void updateSwitchScene_();
		void updateRemoveScene_();

	public:
		void setArrow( Arrow * _arrow ) override;
		Arrow * getArrow() const override;

	public:
		void calcGlobalMouseWorldPosition( const mt::vec2f & _screenPoint, mt::vec2f & _worldPoint ) override;
		void calcGlobalMouseWorldDeltha( const mt::vec2f & _screenPoint, const mt::vec2f & _screenDeltha, mt::vec2f & _worldDeltha ) override;

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
		void initializeRenderResources();
		void finalizeRenderResources();

    public:
		void tick( float _timing );
		bool update();


	public:
		float getTime() const override;

	public:
		void render();
		
	public:
		bool handleKeyEvent( const mt::vec2f & _point, unsigned int _key, unsigned int _char, bool _isDown, bool _repeating ) override;

	public:
		bool handleMouseButtonEvent( unsigned int _touchId, const mt::vec2f & _point, unsigned int _button, bool _isDown ) override;
		bool handleMouseButtonEventBegin( unsigned int _touchId, const mt::vec2f & _point, unsigned int _button, bool _isDown ) override;
		bool handleMouseButtonEventEnd( unsigned int _touchId, const mt::vec2f & _point, unsigned int _button, bool _isDown ) override;
		bool handleMouseMove( unsigned int _touchId, const mt::vec2f & _point, float _x, float _y ) override;
		bool handleMouseWheel( unsigned int _touchId, const mt::vec2f & _point, int _wheel ) override;


		void onFocus( bool _focus );

		void onAppMouseLeave();
		void onAppMouseEnter();
		void onAppMousePosition( const mt::vec2f & _point );

		void onFullscreen( const Resolution & _resolution, bool _fullscreen );
        void onFixedContentResolution( const Resolution & _resolution, bool _fixed );

    protected:
        void invalidateResolution_( const Resolution & _resolution );
		
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
        Game * m_game;

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
		
		ConstString m_switchSceneName;

		bool m_switchScene;
		bool m_destroyOldScene;
		bool m_destroyAfterSwitch;
		bool m_restartScene;

		bool m_removeScene;

		Resolution m_contentResolution;
		Resolution m_currentResolution;
		
		PyObject* m_changeSceneCb;
		PyObject* m_removeSceneCb;

		float m_time;

		uint32_t m_fps;

//#	ifndef MENGE_MASTER_RELEASE
	public:
		void toggleDebugText() override;

	protected:
		uint32_t m_showDebugText;
		TextField * m_debugText;
		Camera2D * m_debugCamera2D;
//#	endif
	};
}
