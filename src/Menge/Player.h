#	pragma once

#	include "Holder.h"

#	include "InputHandler.h"

#	include <string>

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
		void setCurrentScene( const std::string & _scene );
		Scene * getCurrentScene();

		void setArrow( Arrow * _arrow );

	public:
		void setRenderCamera2D( Camera2D * _camera);
		Camera2D * getRenderCamera2D();

		void setRenderCamera3D( Camera3D * _camera);
		Camera3D * getRenderCamera3D();

	public:
		void init();
		void update( float _timing );
		void render();

		bool handleKeyEvent( size_t _key, bool _isDown ) override;
		bool handleMouseButtonEvent( size_t _button, bool _isDown ) override;
		bool handleMouseMove( float _x, float _y, float _whell ) override;

		void debugRender();
	
	private:
		Avatar * m_avatar;
		Scene * m_scene;
		Arrow * m_arrow;

		Camera2D * m_renderCamera2D;
		Camera3D * m_renderCamera3D;
	};
}