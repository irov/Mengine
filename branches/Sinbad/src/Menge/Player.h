#	pragma once

#	include "Core/Holder.h"

#	include "InputHandler.h"

#	include "Core/Resolution.h"

#	include "Math/vec3.h"

#	include <set>

#	include <pybind/types.hpp>

namespace Menge
{	
	class Scene;
	class Avatar;
	class Arrow;
	class TextField;

	class Camera2D;
	class Camera3D;
	class MousePickerSystem;
	class ScheduleManager;

	class Player
		: public InputHandler
		, public Holder<Player>
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
		bool isChangedScene() const;

	public:
		bool init( const Resolution & _contentResolution );
		void tick( float _timing );
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
		void scheduleRemove( std::size_t _id );
		void scheduleRemoveAll();
		void scheduleFreeze( std::size_t _id, bool _freeze );
		void scheduleSetUpdatable( bool _updatable );

		void addCallback( PyObject* _callback, PyObject* _node, bool _endFlag );
		void setCursorMode( bool _mode );

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

		typedef std::list<GlobalMouseHandler *> TSetGlobalMouseHandler;
		TSetGlobalMouseHandler m_setGlobalMouseHandler;

		typedef std::list<GlobalKeyHandler *> TSetGlobalKeyHandler;
		TSetGlobalKeyHandler m_setGlobalKeyHandler;
		
		PyObject* m_setScenePyCb;
		String m_nextSceneName;

		std::size_t m_fps;

		struct CallbackInfo
		{
			PyObject* callback;
			PyObject* node;
			bool endFlag;
		};
		typedef std::vector<CallbackInfo> TCallbackInfoVector;
		TCallbackInfoVector m_callbacks;

#	ifndef MENGE_MASTER_RELEASE
	public:
		void toggleDebugText();

	protected:
		bool m_showDebugText;
		TextField* m_debugText;
#	endif
	};
}
