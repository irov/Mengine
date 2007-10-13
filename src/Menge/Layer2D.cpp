#	include "Layer2D.h"

#	include "Camera2D.h"

#	include "ObjectImplement.h"

#	include "Scene.h"

#	include "Camera2D.h"
#	include "Player.h"

#	include "XmlParser/XmlParser.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT(Layer2D);
	//////////////////////////////////////////////////////////////////////////
	Layer2D::Layer2D()
	: m_factorParallax(1.f,1.f)
	{
	}
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

		if( main == 0 )
		{
			printf("this scene [%s] is not valid: not have 'main' layer!!\n", m_scene->getName().c_str() );
			throw std::exception("this scene is not valid: not have 'main' layer!!");
		}

		const mt::vec2f & main_size = main->getSize();

		mt::vec2f viewport_size = ( _viewPort.end - _viewPort.begin ) * 0.5f;
		mt::vec2f viewport_middle = ( _viewPort.begin + _viewPort.end ) * 0.5f;

		Camera2D * camera = Holder<Player>::hostage()
			->getRenderCamera2D();

		mt::vec2f camera_position = camera->getWorldPosition();

		if( false
			|| camera_position.x - viewport_size.x < 0.f 
			|| camera_position.y - viewport_size.y < 0.f )
		{
			camera_position = viewport_size;
		}

		if( false
			|| camera_position.x +  viewport_size.x > main_size.x 
			|| camera_position.y +  viewport_size.y > main_size.y )
		{
			camera_position = main_size - viewport_size;
		}

		mt::vec2f main_paralax_size = main_size - viewport_size * 2.f;

		mt::vec2f main_paralax_position = camera_position - viewport_size;

		float parallax_factor_x = (main_paralax_size.x > 0.0001f) ? main_paralax_position.x / main_paralax_size.x : 0.f;
		float parallax_factor_y = (main_paralax_size.y > 0.0001f) ? main_paralax_position.y / main_paralax_size.y : 0.f;

		const mt::vec2f & layer_size = getSize();

		mt::vec2f paralax_size = layer_size - viewport_size * 2.f;
		

		viewport_middle.x = paralax_size.x * parallax_factor_x + viewport_size.x;
		viewport_middle.y = paralax_size.y * parallax_factor_y + viewport_size.y;

		//const mt::vec2f & size = getSize();

		//float parallax_x = main_size.x / size.x;
		//float parallax_y = main_size.y / size.y;

		//viewport_middle.x *= m_factorParallax.x;
		//viewport_middle.y *= m_factorParallax.y;

		m_viewPort.begin = viewport_middle - viewport_size;
		m_viewPort.end = viewport_middle + viewport_size;
		
		return m_viewPort;
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer2D::update( size_t _timing, const Viewport & _viewport )
	{
		const Viewport & viewport = updateViewport( _viewport );

		SceneNode2D::update( _timing, viewport );
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
	bool Layer2D::handleMouseMove( int _x, int _y, int _whell )
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
	bool Layer2D::_activate()
	{
		if( m_main )
		{
			m_scene->setMainLayer( this );
		}

		return true;
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