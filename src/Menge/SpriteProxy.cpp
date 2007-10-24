#	include "SpriteProxy.h" 

#	include "ObjectImplement.h"

#	include "RenderEngine.h"

#	include "Viewport.h"

#	include "XmlParser/XmlParser.h"

#	include "Interface/RenderSystemInterface.h"

#	include "ResourceImage.h"

#	include "ResourceManager.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	SpriteProxy::SpriteProxy(Sprite * _spr)
	: m_sprite(_spr)
	{}
	//////////////////////////////////////////////////////////////////////////
	SpriteProxy::~SpriteProxy()
	{}
	///////////////////////////////////////////////////////////////////////////
	bool SpriteProxy::isVisible(const Viewport & _viewPort)
	{
		return	m_sprite->isVisible(_viewPort);
	}
	//////////////////////////////////////////////////////////////////////////
	void SpriteProxy::setImageIndex( size_t _index )
	{
		return	m_sprite->setImageIndex(_index);
	}
	//////////////////////////////////////////////////////////////////////////
	size_t SpriteProxy::getImageIndex() const
	{
		return	m_sprite->getImageIndex();
	}
	//////////////////////////////////////////////////////////////////////////
	void SpriteProxy::setImageResource( const std::string & _name )
	{
		return	m_sprite->setImageResource(_name);
	}
	//////////////////////////////////////////////////////////////////////////
	const std::string & SpriteProxy::getImageResource() const
	{
		return	m_sprite->getImageResource();
	}
	//////////////////////////////////////////////////////////////////////////
	bool SpriteProxy::_activate()
	{
		return	m_sprite->_activate();
	}
	//////////////////////////////////////////////////////////////////////////
	void SpriteProxy::_deactivate()
	{
		return	m_sprite->_deactivate();
	}
	//////////////////////////////////////////////////////////////////////////
	void SpriteProxy::loader(TiXmlElement * _xml)
	{
		return	m_sprite->loader(_xml);
	}
	//////////////////////////////////////////////////////////////////////////
	void SpriteProxy::_render( const mt::mat3f & _rwm, const Viewport & _viewPort )
	{
		return	m_sprite->_render(_rwm, _viewPort);
	}
	//////////////////////////////////////////////////////////////////////////
	void SpriteProxy::_debugRender()
	{
		return	m_sprite->_debugRender();
	}
}