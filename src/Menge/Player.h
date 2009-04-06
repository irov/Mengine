#	pragma once

#	include "Holder.h"

#	include "InputHandler.h"

#	include "Resolution.h"

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
	class Avatar;
	class Arrow;

	class Camera2D;
	class Camera3D;
	class MousePickerSystem;
	class ScheduleManager;

	class RenderObject;

	class Player
		: public InputHandler
	{
	public:
		Player();
		~Player();

	public:
		void setCurrentScene( const String& _scene, bool _destroyOld = false );
		Scene * getCurrentScene();
		void setCurrentSceneCb( const String& _scene, PyObject* _cb );

		void setArrow( Arrow * _arrow );
		Arrow * getArrow();

	public:
		void setRenderCamera2D( Camera2D * _camera);
		Camera2D * getRenderCamera2D();
		void setCamera2DPosition( const mt::vec2f & _pos );

		void updateChangeScene();

	public:
		bool init( const Resolution & _contentResolution );
		void update( float _timing );
		void render( unsigned int _debugMask );

		bool handleKeyEvent( unsigned int _key, unsigned int _char, bool _isDown ) override;
		bool handleMouseButtonEvent( unsigned int _button, bool _isDown ) override;
		bool handleMouseMove( float _x, float _y, int _whell ) override;
		void onFocus( bool _focus );

		void regGlobalMouseEventable( GlobalMouseHandler * _handler );
		void unregGlobalMouseEventable( GlobalMouseHandler * _handler );

		void regGlobalKeyEventable( GlobalKeyHandler * _handler );
		void unregGlobalKeyEventable( GlobalKeyHandler * _handler );

		void onMouseLeave();
		void onMouseEnter();

		std::size_t schedule( float _timing, PyObject * _func );
		std::size_t timerSchedule( float _timing, PyObject* _func );
		void scheduleRemove( std::size_t _id );
		void scheduleRemoveAll();
		void scheduleFreeze( std::size_t _id, bool _freeze );
		void scheduleSetUpdatable( bool _updatable );

	private:
		Avatar * m_avatar;
		Scene * m_scene;
		Arrow * m_arrow;
		ScheduleManager* m_scheduleManager;
		
		Camera2D * m_renderCamera2D;
		MousePickerSystem* m_mousePickerSystem;
		
		bool m_arrowHided;
		bool m_switchScene;
		bool m_destroyOldScene;
		bool m_restartScene;

		typedef std::set<GlobalMouseHandler *> TSetGlobalMouseHandler;
		TSetGlobalMouseHandler m_setGlobalMouseHandler;

		typedef std::set<GlobalKeyHandler *> TSetGlobalKeyHandler;
		TSetGlobalKeyHandler m_setGlobalKeyHandler;
		
		PyObject* m_setScenePyCb;
		String m_nextSceneName;
		RenderObject* m_renderObjectPlayer;
	};
}
