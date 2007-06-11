#	pragma once

#	include "Holder.h"

#	include <string>

namespace Menge
{	
	class Chapter;
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
		void setChapter( const std::string & _name );
		Chapter * getChapter();

		void gotoScene( const std::string & _name );
		Scene * getScene();

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
		Chapter * m_chapter;
		Scene * m_scene;
		Arrow * m_arrow;

		Camera2D * m_renderCamera;
	};
}