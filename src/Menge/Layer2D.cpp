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

		Layer * main = m_scene->getMainLayer();

		const mt::vec2f & main_size = main->getSize();

		mt::vec2f viewport_size = ( _viewPort.end - _viewPort.begin ) * 0.5f;
		mt::vec2f viewport_middle = ( _viewPort.begin + _viewPort.end ) * 0.5f;

		//if( viewport_middle - viewport_size < 0.f )
		//{
		//	viewport_middle = viewport_size;
		//}

		//if( viewport_middle + viewport_size > main_size )
		//{
		//	viewport_middle = main_size - viewport_size;
		//}

		//const mt::vec2f & size = getSize();

		//float parallax_x = main_size.x / size.x;
		//float parallax_y = main_size.y / size.y;

		viewport_middle.x *= m_factorParallax.x;
		viewport_middle.y *= m_factorParallax.y;

		m_viewPort.begin = viewport_middle - viewport_size;
		m_viewPort.end = viewport_middle + viewport_size;
		
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
		Layer::loader(_xml);

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