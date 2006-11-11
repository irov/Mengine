#	pragma once

#	include "Visitor.h"

#	include "NodeImpl.h"

#	include	"SpriteProxy.h"

namespace	Menge
{
	class SpriteContainer
		: public NodeImpl
	{
		OBJECT_DECLARE(SpriteContainer)
	public:
		void	addSprite(Sprite*	_spr);
		void	setAllLooped(bool _looped);
		void	start();
		void	stop();
		void	setLooped(bool _looped);
		void	nextSprite();
	public:
		void	_visit(Visitor * _visitor);
		void	loader(TiXmlElement *xml);
	private:
		typedef std::list<SpriteProxy*> TListSprite;
		TListSprite	m_sprites;

		TListSprite::iterator	m_currentSprite;
	};
};