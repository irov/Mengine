#	pragma once

#	include "Node.h"

#	include	"SpriteProxy.h"

class SpriteContainer
	: public Node
{
public:
	SpriteContainer(){}

	~SpriteContainer(){}

	void	addSprite(Sprite*	_spr)
	{
		m_sprites.push_back(new SpriteProxy(_spr,_spr));
	}

	void	setAllLooped(bool _looped)
	{
		for(TListSprite::iterator	it = m_sprites.begin(); it != m_sprites.end(); ++it)
		{
			(*it)->setLooped(_looped);
		}
	}

	void	render()
	{
		if(m_sprites.empty())	return;
		(*m_currentSprite)->render();
	}

	void	start()
	{
		if(m_sprites.empty())	return;
		(*m_currentSprite)->play();
	}

	void	nextSprite()
	{
		if(m_sprites.empty())	return;
		++m_currentSprite;
		if (m_currentSprite == m_sprites.end())
		{
			m_currentSprite = m_sprites.begin();
		}
	}

	void	loader(TiXmlElement *xml)
	{}

private:
	typedef std::list<SpriteProxy*> TListSprite;
	TListSprite	m_sprites;

	TListSprite::iterator	m_currentSprite;
};