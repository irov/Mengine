#include "SpriteProxy.h"

namespace Menge
{
	SpriteProxy::SpriteProxy(Sprite * _spr) : NodeProxy(_spr), m_Sprite(_spr)
	{
	}

	void SpriteProxy::stop()
	{
		m_Sprite->stop();
	}

	void SpriteProxy::play()
	{
		m_Sprite->play();
	}

	void SpriteProxy::setLooped(bool _looped)
	{
		m_Sprite->setLooped(_looped);
	}

	bool SpriteProxy::getLooped() const
	{
		return m_Sprite->getLooped();
	}

	void SpriteProxy::setFirstFrame()
	{
		m_Sprite->setFirstFrame();
	}

	void SpriteProxy::loader(TiXmlElement *xml)
	{
		m_Sprite->loader(xml);
	}

	void SpriteProxy::render( const ViewPort & _viewPort )
	{
		m_Sprite->render(_viewPort);
	}
};