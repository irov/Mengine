#	pragma once

#	include "Holder.h"

#	include "InputHandler.h"

#	include "Resolution.h"

#	include "Math/vec3.h"
#	include <set>

namespace Menge
{	
	class Scene;
	class Avatar;
	class Arrow;

	class Camera2D;
	class Camera3D;
	class MousePickerSystem;

	class Player
		: public InputHandler
	{
	public:
		Player();
		~Player();

	public:
		void setCurrentScene( const String& _scene, bool _destroyOld = false );
		Scene * getCurrentScene();

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

		void regGlobalMouseEventable( GlobalMouseHandler * _handler );
		void unregGlobalMouseEventable( GlobalMouseHandler * _handler );

		void regGlobalKeyEventable( GlobalKeyHandler * _handler );
		void unregGlobalKeyEventable( GlobalKeyHandler * _handler );

		void onMouseLeave();
		void onMouseEnter();
	
	private:
		Avatar * m_avatar;
		Scene * m_scene;
		Arrow * m_arrow;
		
		Camera2D * m_renderCamera2D;
		MousePickerSystem* m_mousePickerSystem;
		
		bool m_arrowHided;
		bool m_switchScene;
		bool m_destroyOldScene;
		bool m_restartScene;
		Scene * m_nextScene;

		typedef std::set<GlobalMouseHandler *> TSetGlobalMouseHandler;
		TSetGlobalMouseHandler m_setGlobalMouseHandler;

		typedef std::set<GlobalKeyHandler *> TSetGlobalKeyHandler;
		TSetGlobalKeyHandler m_setGlobalKeyHandler;

	};
}