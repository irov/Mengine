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

	class Camera2D;

	class MousePickerSystem;
	class GlobalHandleSystem;
	class ScheduleManager;

	class Player
		: public InputHandler
		, public Holder<Player>
	{
	public:
		Player();
		~Player();

	public:
		void setCurrentScene( const ConstString& _scene, bool _destroyOld = false );
		Scene * getCurrentScene();
		void setCurrentSceneCb( const ConstString& _scene, PyObject* _cb );

		void setArrow( Arrow * _arrow );
		Arrow * getArrow();

		ScheduleManager * getScheduleManager();

	public:
		void setRenderCamera2D( Camera2D * _camera);
		Camera2D * getRenderCamera2D();
		void setCamera2DPosition( const mt::vec2f & _pos );

		bool updateChangeScene();
		bool isChangedScene() const;

	public:
		bool init( Arrow * _arrow, const Resolution & _contentResolution );
		void tick( float _timing );
		void render( unsigned int _debugMask );

		bool handleKeyEvent( unsigned int _key, unsigned int _char, bool _isDown ) override;
		bool handleMouseButtonEvent( unsigned int _button, bool _isDown ) override;
		bool handleMouseButtonEventEnd( unsigned int _button, bool _isDown ) override;
		bool handleMouseMove( float _x, float _y, int _whell ) override;
		void onFocus( bool _focus );

		void onMouseLeave();
		void onMouseEnter();

		void addCallback( PyObject* _callback, PyObject* _node, bool _endFlag );
		void setCursorMode( bool _mode );

	protected:
		void renderArrow_( unsigned int _debugMask );

	private:
		Scene * m_scene;
		Arrow * m_arrow;
		
		Camera2D * m_renderCamera2D;

		MousePickerSystem * m_mousePickerSystem;
		GlobalHandleSystem * m_globalHandleSystem;

		bool m_arrowHided;
		bool m_switchScene;
		bool m_destroyOldScene;
		bool m_restartScene;
		
		PyObject* m_setScenePyCb;
		ConstString m_nextSceneName;

		std::size_t m_fps;

		struct CallbackInfo
		{
			PyObject* callback;
			PyObject* node;
			bool endFlag;
		};
		typedef std::vector<CallbackInfo> TCallbackInfoVector;
		TCallbackInfoVector m_callbacks;

		ScheduleManager* m_scheduleManager;

#	ifndef MENGE_MASTER_RELEASE
	public:
		void toggleDebugText();

	protected:
		bool m_showDebugText;
		TextField* m_debugText;
#	endif
	};
}
