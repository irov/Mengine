#include "SpriteContainer.h"

namespace	Menge
{
	void	SpriteContainer::addSprite(Sprite*	_spr)
	{
		m_sprites.push_back(new SpriteProxy(_spr));
	}

	void	SpriteContainer::setAllLooped(bool _looped)
	{
		for(TListSprite::iterator	it = m_sprites.begin(); it != m_sprites.end(); ++it)
		{
			(*it)->setLooped(_looped);
		}
	}

	void	SpriteContainer::start()
	{
		if(m_sprites.empty())	return;

		(*m_currentSprite)->play();
	}

	void	SpriteContainer::stop()
	{
		if(m_sprites.empty())	return;

		(*m_currentSprite)->stop();
	}

	void	SpriteContainer::setLooped(bool _looped)
	{
		if(m_sprites.empty())	return;

		(*m_currentSprite)->setLooped(_looped);
	}

	void	SpriteContainer::_visit(Visitor * _visitor)
	{
		_visitor->apply( *m_currentSprite );
	}

	void	SpriteContainer::nextSprite()
	{
		if(m_sprites.empty())	return;

		++m_currentSprite;

		if (m_currentSprite == m_sprites.end())
		{
			m_currentSprite = m_sprites.begin();
		}
	}

	void	SpriteContainer::loader(TiXmlElement *xml)
	{}
};