#	pragma once

#	include "Keeper.h"

namespace Menge
{
	class Chapter;
	class Scene;
	class Arrow;

	class Player
	{
	public:
		Player();

	public:
		void setChapter( Chapter *_chapter );
		void setScene( Scene * _scene );

	public:
		void update( float _timing );
		void render();

		void debugRender();
		
	private:
		Chapter * m_chapter;
		Scene * m_scene;
		Arrow *m_arrow;
	};
}