#	include "Layer2DLoop.h"

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
	OBJECT_IMPLEMENT(Layer2DLoop);
	//////////////////////////////////////////////////////////////////////////
	Layer2DLoop::Layer2DLoop()
		: m_factorParallax(1.f,1.f)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer2DLoop::setOffsetPosition( const mt::vec2f & _offset )
	{
		setLocalPosition( _offset );
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer2DLoop::loader( XmlElement * _xml )
	{
		Layer::loader(_xml);

		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_ATTRIBUTE_NODE_METHOD( "Parallax", "Factor", &Layer2D::setParallaxFactor );
		//	XML_CASE_ATTRIBUTE_NODE_METHOD( "Scrollable", "Value", &Layer2D::setScrollable );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	bool Layer2DLoop::_activate()
	{
		if( m_scene == 0 )
		{
			return false;
		}

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
				const mt::box2f & sprite_bbox = _node->getWorldBoundingBox();

				if( m_viewport.end.x < sprite_bbox.vb.x ) 
				{
					if( sprite_bbox.ve.x < m_size.x )
					{
						return;
					}

					float segment_x = sprite_bbox.ve.x - m_size.x;

					if( segment_x < m_viewport.begin.x )
					{
						return;
					}

					Viewport viewport = m_viewport;
					viewport.begin += mt::vec2f( m_size.x, 0.f );
					viewport.end += mt::vec2f( m_size.x, 0.f );
					_node->renderSelf( viewport );
				}
				else if( m_viewport.begin.x > sprite_bbox.ve.x ) 
				{
					if( m_viewport.end.x < m_size.x )
					{
						return;
					}

					float segment_x = m_viewport.end.x - m_size.x;

					if( segment_x < sprite_bbox.vb.x )
					{
						return;
					}

					Viewport viewport = m_viewport;
					viewport.begin += mt::vec2f( -m_size.x, 0.f );
					viewport.end += mt::vec2f( -m_size.x, 0.f );
					_node->renderSelf( viewport );
				}

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
	void Layer2DLoop::render( const Viewport & _viewport )
	{
		Holder<RenderEngine>::hostage()
			->beginLayer2D();

		mt::vec2f viewport_size = _viewport.end - _viewport.begin;

		Viewport viewport;

		viewport.begin = _viewport.begin;

		viewport.begin.x *= m_factorParallax.x;
		viewport.begin.y *= m_factorParallax.y;

		viewport.end = viewport.begin + viewport_size;

		VisitorRenderLayer2D visitorRender( viewport, m_size );

		visitChildren( &visitorRender );

		Holder<RenderEngine>::hostage()
			->endLayer2D();
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer2DLoop::_addChildren( Node * _node )
	{
		_node->setLayer( this );
	}
}