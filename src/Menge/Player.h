#	pragma once

#	include "Holder.h"

#	include <string>

namespace Menge
{	
	class Scene;
	class Avatar;
	class Arrow;
	class Camera2D;

	class Player
	{
	public:
		Player();
		~Player();

	public:
		void setCurrentScene( const std::string & _scene );
		Scene * getCurrentScene();

		void setArrow( Arrow * _arrow );

	public:
		void setRenderCamera( Camera2D * _camera);
		Camera2D * getRenderCamera();

	public:
		void init();
		void update( float _timing );
		void render();

		void debugRender();
	
	private:
		Avatar * m_avatar;
		Scene * m_scene;
		Arrow * m_arrow;

		Camera2D * m_renderCamera;
	};
}