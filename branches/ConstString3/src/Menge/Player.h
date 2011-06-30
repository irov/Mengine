#	pragma once

#	include "Core/ConstString.h"
#	include "InputHandler.h"

#	include "Core/Holder.h"
#	include "Core/Resolution.h"

#	include "Math/vec3.h"
#	include <set>

extern "C" 
{ 
	struct _object; 
	typedef _object PyObject;
}

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
		void setCurrentScene( const ConstString& _scene, bool _destroyOld, bool _destroyAfterSwitch, PyObject* _cb );
		Scene * getCurrentScene();

		bool isChangedScene() const;

	protected:
		bool updateChangeScene_();

	public:
		void setArrow( Arrow * _arrow );
		Arrow * getArrow();

		ScheduleManager * getScheduleManager();

	public:
		void setRenderCamera2D( Camera2D * _camera);
		Camera2D * getRenderCamera2D();
		void setCamera2DPosition( const mt::vec2f & _pos );

	public:
		bool initialize( Arrow * _arrow, const Resolution & _contentResolution, const Resolution & _currentResolution );
		void finalize();

		void tick( float _timing );
		bool update();

		void render( unsigned int _debugMask );

		bool handleKeyEvent( unsigned int _key, unsigned int _char, bool _isDown ) override;
		bool handleMouseButtonEvent( unsigned int _button, bool _isDown ) override;
		bool handleMouseButtonEventEnd( unsigned int _button, bool _isDown ) override;
		bool handleMouseMove( float _x, float _y, int _whell ) override;
		void onFocus( bool _focus );

		void onAppMouseLeave();
		void onAppMouseEnter();

		void onFullscreen( const Resolution & _resolution, bool _fullscreen );

		void setCursorMode( bool _mode );

	public:
		Join * addJoin( Node * _left, Node * _right );
		void removeJoin( Join * _join );
		bool isJoin( Node * _left, Node * _right );

		void getJoins( Node * _node, TVectorNode & _joins );

	protected:
		void updateJoins_();

	protected:
		typedef std::vector<Join *> TVectorJoins;
		TVectorJoins m_joins;

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

		bool m_arrowHided;
		
		ConstString m_switchSceneName;

		bool m_switchScene;
		bool m_destroyOldScene;
		bool m_destroyAfterSwitch;
		bool m_restartScene;

		Resolution m_contentResolution;
		Resolution m_currentResolution;
		
		PyObject* m_setScenePyCb;

		std::size_t m_fps;

#	ifndef MENGE_MASTER_RELEASE
	public:
		void toggleDebugText();

	protected:
		bool m_showDebugText;
		TextField* m_debugText;
#	endif
	};
}
