#	include "Layer2D.h"

#	include "Camera2D.h"

#	include "ObjectImplement.h"

#	include "Scene.h"

#	include "Camera2D.h"
#	include "Player.h"

#	include "RenderEngine.h"

#	include "LogEngine.h"

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
	void Layer2D::updateViewport()
	{
		Layer * main = m_scene->getMainLayer();

		if( main == 0 )
		{
			MENGE_LOG("this scene [%s] is not valid: not have 'main' layer!!\n", m_scene->getName().c_str() );
			throw std::exception("this scene is not valid: not have 'main' layer!!");
		}

		const mt::vec2f & main_size = main->getSize();

		//mt::vec2f viewport_size( 512.f, 384.f );
		//mt::vec2f viewport_middle( 512.f, 384.f );

		mt::vec2f viewport_size = mt::vec2f( 512.f, 384.f );/*( m_viewport.end - m_viewport.begin ) * 0.5f*/
		mt::vec2f viewport_middle = ( m_viewport.begin + m_viewport.end ) * 0.5f;

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

		m_viewport.begin = viewport_middle - viewport_size;
		m_viewport.end = viewport_middle + viewport_size;

		////return m_viewPort;
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer2D::update( float _timing )
	{
		updateViewport();

		NodeCore::update( _timing );
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
	void Layer2D::setOffsetPosition( const mt::vec2f & _offset )
	{
		setLocalPosition( _offset );
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer2D::loader( TiXmlElement * _xml )
	{
		NodeCore::loader(_xml);
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
		if( m_scene == 0 )
		{
			return false;
		}

		if( m_main )
		{
			m_scene->setMainLayer( this );
		}

		m_viewport.begin = mt::vec2f( 0.f, 0.f );
		m_viewport.end = mt::vec2f( 1024.f, 768.f );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Layer2D::_renderBegin()
	{
		Holder<RenderEngine>::hostage()
			->beginLayer();

		Holder<RenderEngine>::hostage()
			->setRenderViewport( m_viewport );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer2D::_renderEnd()
	{
		Holder<RenderEngine>::hostage()
			->endLayer();
	}
}