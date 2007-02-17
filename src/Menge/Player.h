#	pragma once

#	include "Holder.h"

namespace Menge
{
	class Avatar;
	class Scene;
	class Arrow;

	class Player
	{
	public:
		Player();
		~Player();

	public:
		void setScene( Scene * _scene );
		Scene * getScene();

		void setArrow( Arrow * _arrow );

	public:
		void init();
		void update( float _timing );
		void render();

		void debugRender();
		
	private:
		Avatar * m_avatar;
		Scene * m_scene;
		Arrow * m_arrow;
	};
}