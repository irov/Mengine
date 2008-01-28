#	include "Layer2D.h"

#	include "ObjectImplement.h"

#	include "XmlEngine.h"

#	include "Scene.h"

#	include "Camera2D.h"

#	include "Player.h"

#	include "RenderEngine.h"

#	include "LogEngine.h"
#	include "Game.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	OBJECT_IMPLEMENT(Layer2D);
	//////////////////////////////////////////////////////////////////////////
	Layer2D::Layer2D()
	: m_factorParallax(1.f,1.f)
	, m_reRender( false )
	, m_scrollable( false )
	, m_needReRender( false )
	, m_viewportOffset( 0.0f, 0.0f )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	Viewport * Layer2D::getViewport()
	{
		return &m_viewport;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f& Layer2D::getViewportOffset() const
	{
		return m_viewportOffset;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Layer2D::isScrollable()
	{
		return m_scrollable;
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer2D::setParallaxFactor( const mt::vec2f & _factor )
	{
		m_factorParallax = _factor;
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer2D::updateViewport()
	{
		Layer * main = m_scene->getMainLayer();

		if( main == 0 )
		{
			MENGE_LOG("this scene [%s] is not valid: not have 'main' layer!!\n", m_scene->getName().c_str() );
			throw std::exception("this scene is not valid: not have 'main' layer!!");
		}

		const mt::vec2f & main_size = main->getSize();

		mt::vec2f viewport_size = ( m_viewport.end - m_viewport.begin ) * 0.5f;
		mt::vec2f viewport_middle = ( m_viewport.begin + m_viewport.end ) * 0.5f;

		Camera2D * camera = Holder<Player>::hostage()
			->getRenderCamera2D();

		mt::vec2f camera_position = camera->getWorldPosition();

		if( /*camera_position.x - viewport_size.x < 0.f 
			||*/ camera_position.y - viewport_size.y < 0.f )
		{
			camera_position.y = viewport_size.y;
		}
		else if( /*camera_position.x +  viewport_size.x > main_size.x 
			||*/ camera_position.y +  viewport_size.y > main_size.y )
		{
			camera_position.y = main_size.y - viewport_size.y;
		}

		mt::vec2f main_paralax_size = main_size - viewport_size * 2.f;

		mt::vec2f main_paralax_position = camera_position - viewport_size;

		float parallax_factor_x = (main_paralax_size.x > 0.0001f) ? main_paralax_position.x / main_paralax_size.x : 0.f;
		float parallax_factor_y = (main_paralax_size.y > 0.0001f) ? main_paralax_position.y / main_paralax_size.y : 0.f;

		mt::vec2f paralax_size = m_size - viewport_size * 2.f;

		viewport_middle.x = paralax_size.x * parallax_factor_x + viewport_size.x;
		viewport_middle.y = paralax_size.y * parallax_factor_y + viewport_size.y;

		m_viewport.begin = viewport_middle - viewport_size;
		m_viewport.end = viewport_middle + viewport_size;

		//
		if( !m_scrollable ) return;

		if( m_viewport.begin.x < -m_size.x || m_viewport.begin.x > m_size.x )
		{
			m_viewport.begin.x += m_size.x * ::floorf( m_viewport.begin.x / -m_size.x );
			m_viewport.end.x = m_viewport.begin.x + viewport_size.x * 2.0f;
		}

		if( !m_reRender )
		{
			if( m_viewport.begin.x < 0.0f )
			{
				m_viewportOffset.x = m_size.x;
				m_viewportOffset.y = 0.0f;
				// notify re-render
				m_needReRender = true;
			}
			else if( m_viewport.end.x > m_size.x )
			{
				m_viewportOffset.x = -m_size.x;
				m_viewportOffset.y = 0.0f;
				// notify re-render
				m_needReRender = true;
			}	
		}	
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer2D::update( float _timing )
	{
		updateViewport();

		Holder<RenderEngine>::hostage()
			->setRenderViewport( m_viewport );

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
	void Layer2D::loader( XmlElement * _xml )
	{
		NodeCore::loader(_xml);
		Layer::loader(_xml);

		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_NODE("Parallax")
			{
				//mt::vec2f offset;

				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE_MEMBER( "Factor", &Layer2D::setParallaxFactor );
					//setParallaxFactor( offset );	
				}
			}

			XML_CASE_NODE("Scrollable")
			{
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE("Value", m_scrollable);
				}
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

		if( m_reRender )
		{
			Viewport viewport;
			viewport.begin = m_viewport.begin + m_viewportOffset;
			viewport.end = m_viewport.end + m_viewportOffset;
			//m_viewport.begin += m_viewportOffset;
			//m_viewport.end += m_viewportOffset;	
			Holder<RenderEngine>::hostage()
				->setRenderViewport( viewport );
			return true;
		}

		Holder<RenderEngine>::hostage()
			->setRenderViewport( m_viewport );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer2D::render()
	{
		if( NodeRenderable::isRenderable() == false )
		{
			return;
		}

		if( _renderBegin() )
		{
			_render();

			struct ForeachRender
				: public NodeForeach
			{
				void apply( Node * children ) override
				{
					children->render();
				}
			};

			foreachChildren( ForeachRender() );

			_renderEnd();
		}
		if( m_needReRender )
		{
			m_reRender = true;
			if( _renderBegin() )
			{
				_render();

				struct ForeachRender
					: public NodeForeach
				{
					void apply( Node * children ) override
					{
						children->render();
					}
				};

				foreachChildren( ForeachRender() );

				_renderEnd();
			}
			m_needReRender = false;
			m_reRender = false;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer2D::_render()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	void Layer2D::_renderEnd()
	{
		Holder<RenderEngine>::hostage()
			->endLayer();
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer2D::_addChildren( SceneNode2D * _node )
	{
		_node->setLayer( this );

		// прости господи
		struct ForeachRender
			: public NodeForeach
		{
			Layer2D * layer;
			ForeachRender(Layer2D * _layer)
				: layer(_layer)
			{}

			void apply( Node * children ) override
			{
				dynamic_cast<SceneNode2D*>(children)->setLayer(layer);
			}
		};

		_node->foreachChildren( ForeachRender(this) );
	}
}