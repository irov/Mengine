#	pragma once

#	include "Holder.h"

#	include <string>

namespace Menge
{	
	class Chapter;
	class Scene;
	class Avatar;
	class Arrow;

	class Player
	{
	public:
		Player();
		~Player();

	public:
		void setChapter( const std::string & _name );
		void gotoScene( const std::string & _name );

	public:
		void setChapter( Chapter * _chapter );
		Chapter * getChapter();

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
		Chapter * m_chapter;
		Scene * m_scene;
		Arrow * m_arrow;
	};
}