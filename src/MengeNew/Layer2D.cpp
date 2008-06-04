#	include "Layer2D.h"

#	include "ObjectImplement.h"

#	include "XmlEngine.h"

#	include "Scene.h"

#	include "Camera2D.h"

#	include "Player.h"

#	include "RenderEngine.h"
#	include "PhysicEngine2D.h"

#	include "LogEngine.h"
#	include "Game.h"


#	include "Sprite.h"
#	include "Animation.h"

#	include "VisitorAdapter.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT(Layer2D);
	//////////////////////////////////////////////////////////////////////////
	Layer2D::Layer2D()
	: m_factorParallax(1.f,1.f)
	//, m_reRender( false )
	, m_scrollable( false )
	//, m_needReRender( false )
	//, m_viewportOffset( 0.0f, 0.0f )
	{
	}
	////////////////////////////////////////////////////////////////////////////
	//Viewport * Layer2D::getViewport()
	//{
	//	return &m_viewport;
	//}
	////////////////////////////////////////////////////////////////////////////
	//const mt::vec2f& Layer2D::getViewportOffset() const
	//{
	//	return m_viewportOffset;
	//}
	//////////////////////////////////////////////////////////////////////////
	void Layer2D::setParallaxFactor( const mt::vec2f & _factor )
	{
		m_factorParallax = _factor;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & Layer2D::getParallaxFactor() const
	{
		return m_factorParallax;
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer2D::setScrollable( bool _scrollable )
	{
		m_scrollable = _scrollable;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Layer2D::isScrollable() const
	{
		return m_scrollable;
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer2D::updateViewport()
	{
		//Layer * main = m_scene->getMainLayer();

		//if( main == 0 )
		//{
		//	MENGE_LOG("this scene [%s] is not valid: not have 'main' layer!!\n", m_scene->getName().c_str() );
		//	throw std::exception("this scene is not valid: not have 'main' layer!!");
		//}

		//const mt::vec2f & main_size = main->getSize();

		//mt::vec2f viewport_size = ( m_viewport.end - m_viewport.begin ) * 0.5f;
		//mt::vec2f viewport_middle = ( m_viewport.begin + m_viewport.end ) * 0.5f;

		//Camera2D * camera = Holder<Player>::hostage()
		//	->getRenderCamera2D();

		//mt::vec2f camera_position = camera->getWorldPosition();

		//if( /*camera_position.x - viewport_size.x < 0.f 
		//	||*/ camera_position.y - viewport_size.y < 0.f )
		//{
		//	camera_position.y = viewport_size.y;
		//}
		//else if( /*camera_position.x +  viewport_size.x > main_size.x 
		//	||*/ camera_position.y +  viewport_size.y > main_size.y )
		//{
		//	camera_position.y = main_size.y - viewport_size.y;
		//}

		//mt::vec2f main_paralax_size = main_size - viewport_size * 2.f;

		//mt::vec2f main_paralax_position = camera_position - viewport_size;

		//float parallax_factor_x = (main_paralax_size.x > 0.0001f) ? main_paralax_position.x / main_paralax_size.x : 0.f;
		//float parallax_factor_y = (main_paralax_size.y > 0.0001f) ? main_paralax_position.y / main_paralax_size.y : 0.f;

		//mt::vec2f paralax_size = m_size - viewport_size * 2.f;

		//viewport_middle.x = paralax_size.x * parallax_factor_x + viewport_size.x;
		//viewport_middle.y = paralax_size.y * parallax_factor_y + viewport_size.y;

		//m_viewport.begin = viewport_middle - viewport_size;
		//m_viewport.end = viewport_middle + viewport_size;

		////
		//if( !m_scrollable ) return;

		//m_needReRender = false;
		////if( m_viewport.begin.x < -m_size.x || m_viewport.begin.x > m_size.x )
		//{
		//	float c = ::floorf( m_viewport.begin.x / m_size.x );
		//	m_viewport.begin.x -= m_size.x * c;
		//	m_viewport.end.x = m_viewport.begin.x + viewport_size.x * 2.0f;
		//}

		//if( !m_reRender )
		//{
		//	/*if( m_viewport.begin.x < 0.0f )
		//	{
		//		m_viewportOffset.x = m_size.x;
		//		m_viewportOffset.y = 0.0f;
		//		// notify re-render
		//		m_needReRender = true;
		//	}
		//	else*/ if( m_viewport.end.x > m_size.x )
		//	{
		//		m_viewportOffset.x = -m_size.x;
		//		m_viewportOffset.y = 0.0f;
		//		// notify re-render
		//		m_needReRender = true;
		//	}	
		//}	
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer2D::_update( float _timing )
	{
		//updateViewport();

		//Holder<RenderEngine>::hostage()
		//	->setRenderViewport( m_viewport );

		//if( m_scrollable )
		//{
		//	for( TContainerChildrens::iterator
		//		it = m_childrens.begin(),
		//		it_end = m_childrens.end();
		//	it != it_end;
		//	++it)
		//	{
		//		Node * children = *it;

		//		Node * node = dynamic_cast< SceneNode2D* >( children );

		//		const mt::vec2f& pos = node->getWorldPosition();
		//		const mt::vec2f& size = node->getLayer()->getSize();
		//		if( pos.x > size.x )
		//		{
		//			//pos.x -= m_layer->getSize().x;
		//			node->translate( mt::vec2f( -size.x, 0.0f ));
		//		}
		//		else if( pos.x < 0 )
		//		{
		//			//pos.x += m_layer->getSize().x;
		//			node->translate( mt::vec2f( size.x, 0.0f ));
		//		}
		//	}
		//}

		Node::_update( _timing );
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer2D::setOffsetPosition( const mt::vec2f & _offset )
	{
		setLocalPosition( _offset );
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer2D::loader( XmlElement * _xml )
	{
		Layer::loader(_xml);

		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_ATTRIBUTE_NODE_METHOD( "Parallax", "Factor", &Layer2D::setParallaxFactor );
			XML_CASE_ATTRIBUTE_NODE_METHOD( "Scrollable", "Value", &Layer2D::setScrollable );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Layer2D::_activate()
	{
		if( m_scene == 0 )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer2D::_deactivate()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool Layer2D::_compile()
	{
		//if( m_main )
		//{
		//	m_scene->setMainLayer( this );
		//}
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	class VisitorRenderLayer2D
		: public VisitorAdapter<VisitorRenderLayer2D>
	{
	public:
		VisitorRenderLayer2D( const Viewport & _viewport, const mt::vec2f & _size )
			: m_viewport(_viewport)
			, m_size(_size)
		{
		}

	public:
		void procces( Node * _node )
		{				
			if( _node->isRenderable() == true )
			{
				//const mt::box2f & sprite_bbox = _node->getWorldBoundingBox();

				//if( m_viewport.end.x < sprite_bbox.vb.x ) 
				//{
				//	if( sprite_bbox.ve.x < m_size.x )
				//	{
				//		return;
				//	}

				//	float segment_x = sprite_bbox.ve.x - m_size.x;

				//	if( segment_x < m_viewport.begin.x )
				//	{
				//		return;
				//	}

				//	Viewport viewport = m_viewport;
				//	viewport.begin += mt::vec2f( m_size.x, 0.f );
				//	viewport.end += mt::vec2f( m_size.x, 0.f );
				//	_node->renderSelf( viewport );
				//}
				//else if( m_viewport.begin.x > sprite_bbox.ve.x ) 
				//{
				//	if( m_viewport.end.x < m_size.x )
				//	{
				//		return;
				//	}

				//	float segment_x = m_viewport.end.x - m_size.x;

				//	if( segment_x < sprite_bbox.vb.x )
				//	{
				//		return;
				//	}

				//	Viewport viewport = m_viewport;
				//	viewport.begin += mt::vec2f( -m_size.x, 0.f );
				//	viewport.end += mt::vec2f( -m_size.x, 0.f );
				//	_node->renderSelf( viewport );
				//}

				_node->renderSelf( m_viewport );
			}

			_node->visitChildren( this );
		}

		void procces( Layer * _layer )
		{
			_layer->render( m_viewport );
		}

	protected:
		Viewport m_viewport;
		mt::vec2f m_size;
	};
	//////////////////////////////////////////////////////////////////////////
	void Layer2D::render( const Viewport & _viewport )
	{
		Holder<RenderEngine>::hostage()
			->beginLayer();

		mt::vec2f viewport_size = _viewport.end - _viewport.begin;

		Viewport viewport;

		viewport.begin = _viewport.begin;

		viewport.begin.x *= m_factorParallax.x;
		viewport.begin.y *= m_factorParallax.y;

		viewport.end = viewport.begin + viewport_size;
		
		VisitorRenderLayer2D visitorRender( viewport, m_size );

		visitChildren( &visitorRender );

		Holder<RenderEngine>::hostage()
			->endLayer();
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer2D::_debugRender( const Viewport & _viewport )
	{


	}
	//////////////////////////////////////////////////////////////////////////
	void Layer2D::_addChildren( Node * _node )
	{
		_node->setLayer( this );
	}
	////////////////////////////////////////////////////////////////////////////
	//mt::vec2f Layer2D::screenToLocal( const mt::vec2f& _point )
	//{
	//	return m_viewport.begin + _point;
	//}
	////////////////////////////////////////////////////////////////////////////
	//void Layer2D::setRenderTarget( const std::string& _cameraName )
	//{
	//	m_viewport.setCamera( _cameraName );
	//}
	////////////////////////////////////////////////////////////////////////////
	//bool Layer2D::needReRender()
	//{
	//	return m_needReRender;
	//}
}