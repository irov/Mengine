#	pragma once

#	include "Visitor.h"

#	include "Node.h"

#	include	"SpriteProxy.h"

namespace	Menge
{
	class SpriteContainer
		: public Node
	{
	public:
		void	addSprite(Sprite*	_spr);
		void	setAllLooped(bool _looped);
		void	start();
		void	stop();
		void	setLooped(bool _looped);
		void	_visit(Visitor * _visitor);
		void	nextSprite();
		void	loader(TiXmlElement *xml);
	private:
		typedef std::list<SpriteProxy*> TListSprite;
		TListSprite	m_sprites;

		TListSprite::iterator	m_currentSprite;
	};
};