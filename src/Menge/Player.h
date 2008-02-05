#	pragma once

#	include "Holder.h"

#	include "InputHandler.h"

#	include "Math/vec3.h"

#	include <string>
#	include <set>

namespace Menge
{	
	class Scene;
	class Avatar;
	class Arrow;

	class Camera2D;
	class Camera3D;

	class Player
		: public InputHandler
	{
	public:
		Player();
		~Player();

	public:
		void setCurrentScene( const std::string & _scene, bool _destroyOld = false );
		Scene * getCurrentScene();

		void setArrow( Arrow * _arrow );
		Arrow * getArrow();

		const mt::vec2f & getPositionClick() const;

	public:
		void setRenderCamera2D( Camera2D * _camera);
		Camera2D * getRenderCamera2D();

		void setRenderCamera3D( Camera3D * _camera);
		Camera3D * getRenderCamera3D();

		void setCamera2DPosition( const mt::vec2f & _pos );
		void setCamera3DPosition( const mt::vec3f & _pos );

		void updateChangeScene();

	public:
		void init();
		void update( float _timing );
		void render();

		bool handleKeyEvent( size_t _key, size_t _char, bool _isDown ) override;
		bool handleMouseButtonEvent( size_t _button, bool _isDown ) override;
		bool handleMouseMove( int _x, int _y, int _whell ) override;

		void regGlobalMouseEventable( GlobalMouseHandler * _handler );
		void unregGlobalMouseEventable( GlobalMouseHandler * _handler );

		void regGlobalKeyEventable( GlobalKeyHandler * _handler );
		void unregGlobalKeyEventable( GlobalKeyHandler * _handler );

		void debugRender();
	
	private:
		Avatar * m_avatar;
		Scene * m_scene;
		Arrow * m_arrow;
		
		Camera2D * m_renderCamera2D;
		Camera3D * m_renderCamera3D;

		bool m_switchScene;
		bool m_destroyOldScene;
		Scene * m_nextScene;

		typedef std::set<GlobalMouseHandler *> TSetGlobalMouseHandler;
		TSetGlobalMouseHandler m_setGlobalMouseHandler;

		typedef std::set<GlobalKeyHandler *> TSetGlobalKeyHandler;
		TSetGlobalKeyHandler m_setGlobalKeyHandler;

	};
}