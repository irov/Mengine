#	pragma once

#   include "Interface/PlayerInterface.h"

#	include "Core/ConstString.h"
#	include "Core/Resolution.h"

#   include "Factory/FactoryDefault.h"

#	include "Math/vec3.h"

#	include "pybind/types.hpp"

#   include "TimingManager.h"
#   include "ScheduleManager.h"

namespace Menge
{	
    class Game;
	class Scene;
	class HotSpot;
	class Arrow;
	class TextField;

	class Node;
	class Join;

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
        bool initialize( Arrow * _arrow, const Resolution & _contentResolution, const Resolution & _currentResolution );
        void finalize();

	public:
		bool setCurrentScene( const ConstString& _scene, bool _destroyOld, bool _destroyAfterSwitch, PyObject* _cb );
		bool removeCurrentScene( PyObject * _cb );

		Scene * getCurrentScene() override;

		bool isChangedScene() const;

    protected:
        void updateChangeScene_();
		void updateSwitchScene_();
		void updateRemoveScene_();

	public:
		void setArrow( Arrow * _arrow ) override;
		Arrow * getArrow() const override;

    public:
        MousePickerSystemInterface * getMousePickerSystem() const override;
        GlobalHandleSystemInterface * getGlobalHandleSystem() const override;

    public:
		ScheduleManagerInterface * getScheduleManager() const override;
		ScheduleManagerInterface * getScheduleManagerGlobal() const override;

    public:
		TimingManagerInterface * getTimingManager() const override;
        TimingManagerInterface * getTimingManagerGlobal() const override;

	public:
		void setRenderCamera2D( Camera2D * _camera) override;
		Camera2D * getRenderCamera2D() const override;

    public:
		void setCamera2DPosition( const mt::vec2f & _pos );

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
		bool handleKeyEvent( const mt::vec2f & _point, unsigned int _key, unsigned int _char, bool _isDown ) override;

	public:
		bool handleMouseButtonEvent( unsigned int _touchId, const mt::vec2f & _point, unsigned int _button, bool _isDown ) override;
		bool handleMouseButtonEventBegin( unsigned int _touchId, const mt::vec2f & _point, unsigned int _button, bool _isDown ) override;
		bool handleMouseButtonEventEnd( unsigned int _touchId, const mt::vec2f & _point, unsigned int _button, bool _isDown ) override;
		bool handleMouseMove( unsigned int _touchId, const mt::vec2f & _point, float _x, float _y, int _whell ) override;


		void onFocus( bool _focus );

		void onAppMouseLeave();
		void onAppMouseEnter( const mt::vec2f & _point );

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
		typedef std::vector<Join *> TVectorJoins;
		TVectorJoins m_joins;

		//typedef std::map<Node *, TVectorJoins> TMapJoins;
		//TMapJoins m_nodeJoins;

	protected:
		void renderArrow_( unsigned int _debugMask );

	private:
        Game * m_game;

        ServiceProviderInterface * m_serviceProvider;

		Scene * m_scene;
		Arrow * m_arrow;
		
		Camera2D * m_renderCamera2D;

        MousePickerSystemInterface * m_mousePickerSystem;
        GlobalHandleSystemInterface * m_globalHandleSystem;

		ScheduleManagerInterface * m_scheduleManager;
		ScheduleManagerInterface * m_scheduleManagerGlobal;

        FactoryDefault<ScheduleManager> m_factoryScheduleManager;

		TimingManagerInterface * m_timingManager;
        TimingManagerInterface * m_timingManagerGlobal;

        FactoryDefault<TimingManager> m_factoryTimingManager;

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

		size_t m_fps;

//#	ifndef MENGE_MASTER_RELEASE
	public:
		void toggleDebugText();

	protected:
		bool m_showDebugText;
		TextField* m_debugText;
//#	endif
	};
}
