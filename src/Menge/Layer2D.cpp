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
	{
	}
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
		//	XML_CASE_ATTRIBUTE_NODE_METHOD( "Scrollable", "Value", &Layer2D::setScrollable );
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
	class VisitorRenderLayer2D
		: public VisitorAdapter<VisitorRenderLayer2D>
	{
	public:
		VisitorRenderLayer2D( const Viewport & _viewport )
			: m_viewport(_viewport)
		{
		}

	public:
		void procces( Node * _node )
		{				
			if( _node->isRenderable() == true )
			{
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
	};
	//////////////////////////////////////////////////////////////////////////
	void Layer2D::render( const Viewport & _viewport )
	{
		Holder<RenderEngine>::hostage()
			->beginLayer2D();

		mt::vec2f viewport_size = _viewport.end - _viewport.begin;

		Viewport viewport;

		viewport.begin = _viewport.begin;

		viewport.begin.x *= m_factorParallax.x;
		viewport.begin.y *= m_factorParallax.y;

		viewport.end = viewport.begin + viewport_size;

		VisitorRenderLayer2D visitorRender( viewport );

		visitChildren( &visitorRender );

		Holder<RenderEngine>::hostage()
			->endLayer2D();
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer2D::_addChildren( Node * _node )
	{
		_node->setLayer( this );
	}
}