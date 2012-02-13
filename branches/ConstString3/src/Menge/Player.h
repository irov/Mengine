#	pragma once

#	include "Core/ConstString.h"
#	include "InputHandler.h"

#	include "Core/Holder.h"
#	include "Core/Resolution.h"

#	include "Math/vec3.h"

#	include "pybind/types.hpp"

namespace Menge
{	
	class Scene;
	class HotSpot;
	class Arrow;
	class TextField;

	class Node;
	class Join;

	class Camera2D;

	class MousePickerSystem;
	class GlobalHandleSystem;

	class ScheduleManager;
	class TimingManager;

	class EventManager;
	typedef std::vector<Node*> TVectorNode;
	

	class Player
		: public Holder<Player>
		, public InputHandler		
	{
	public:
		Player();
		~Player();

	public:
		bool setCurrentScene( const ConstString& _scene, bool _destroyOld, bool _destroyAfterSwitch, PyObject* _cb );
		bool removeCurrentScene( PyObject * _cb );

		Scene * getCurrentScene();

		bool isChangedScene() const;

	protected:
		void updateChangeScene_();
		void updateSwitchScene_();
		void updateRemoveScene_();

	public:
		void setArrow( Arrow * _arrow );
		Arrow * getArrow() const;

		ScheduleManager * getScheduleManager() const;
		TimingManager * getTimingManager() const;

	public:
		void setRenderCamera2D( Camera2D * _camera);
		Camera2D * getRenderCamera2D() const;
		void setCamera2DPosition( const mt::vec2f & _pos );

	public:
		bool initialize( Arrow * _arrow, const Resolution & _contentResolution, const Resolution & _currentResolution );
		void finalize();

		void initializeRenderResources();
		void finalizeRenderResources();

		void tick( float _timing );
		bool update();

		void render( unsigned int _debugMask );

		bool handleKeyEvent( const mt::vec2f & _point, unsigned int _key, unsigned int _char, bool _isDown ) override;
		bool handleMouseButtonEvent( const mt::vec2f & _point, unsigned int _button, bool _isDown ) override;
		bool handleMouseButtonEventBegin( const mt::vec2f & _point, unsigned int _button, bool _isDown ) override;
		bool handleMouseButtonEventEnd( const mt::vec2f & _point, unsigned int _button, bool _isDown ) override;
		bool handleMouseMove( const mt::vec2f & _point, float _x, float _y, int _whell ) override;


		void onFocus( bool _focus );

		void onAppMouseLeave();
		void onAppMouseEnter();

		void onFullscreen( const Resolution & _resolution, bool _fullscreen );

		void setCursorMode( bool _mode );

	public:
		Join * addJoin( Node * _left, Node * _right, const mt::vec2f & _offset );
		void removeJoin( Join * _join );
		bool isJoin( Node * _left, Node * _right ) const;

		void getJoins( Node * _node, TVectorNode & _joins ) const;

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
		Scene * m_scene;
		Arrow * m_arrow;
		
		Camera2D * m_renderCamera2D;

		MousePickerSystem * m_mousePickerSystem;
		GlobalHandleSystem * m_globalHandleSystem;
		EventManager * m_eventManager;
		ScheduleManager* m_scheduleManager;
		TimingManager* m_timingManager;

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

		size_t m_fps;

#	ifndef MENGE_MASTER_RELEASE
	public:
		void toggleDebugText();

	protected:
		bool m_showDebugText;
		TextField* m_debugText;
#	endif
	};
}
