#	include "Layer2D.h"

#	include "Camera2D.h"

#	include "ObjectImplement.h"

#	include "Scene.h"

#	include "Player.h"

#	include "XmlParser/XmlParser.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT(Layer2D);
	//////////////////////////////////////////////////////////////////////////
	Layer2D::Layer2D()
	: m_factorParallax(1.f,1.f)
	{}
	//////////////////////////////////////////////////////////////////////////
	void Layer2D::setParallaxFactor( const mt::vec2f & _factor )
	{
		m_factorParallax = _factor;
	}
	//////////////////////////////////////////////////////////////////////////
	const Viewport& Layer2D::updateViewport( const Viewport & _viewPort )
	{
		m_viewPort = _viewPort;
		
		mt::vec2f size = ( _viewPort.end - _viewPort.begin ) * 0.5f;
		mt::vec2f middle = ( _viewPort.begin + _viewPort.end ) * 0.5f;

		middle.x *= m_factorParallax.x;
		middle.y *= m_factorParallax.y;

		m_viewPort.begin = middle - size;
		m_viewPort.end = middle + size;
		
		return m_viewPort;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Layer2D::handleKeyEvent( size_t _key, bool _isDown )
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Layer2D::handleMouseButtonEvent( size_t _button, bool _isDown )
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Layer2D::handleMouseMove( float _x, float _y, float _whell )
	{
		return false;
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer2D::loader( TiXmlElement *_xml)
	{
		SceneNode2D::loader(_xml);

		XML_FOR_EACH_TREE( _xml )
		{
			XML_CHECK_NODE("Parallax")
			{
				mt::vec2f offset;
				XML_VALUE_ATTRIBUTE("Factor", offset);
				setParallaxFactor( offset );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer2D::renderLayer()
	{
		Camera2D * camera = 
			Holder<Player>::hostage()
			->getRenderCamera2D();

		const Viewport & cvp = camera->getViewport();

		const Viewport & viewport = updateViewport( cvp );

		for each( SceneNode2D * node in m_listChildren )
		{
			const mt::mat3f & wm = node->getWorldMatrix();
			node->render( wm, viewport );
		}
	}
}