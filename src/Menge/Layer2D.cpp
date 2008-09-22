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
		: m_factorParallax(0.f,0.f)
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
			XML_CASE_ATTRIBUTE_NODE_METHOD( MENGE_TEXT("Parallax"), MENGE_TEXT("Factor"), &Layer2D::setParallaxFactor );
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
		VisitorRenderLayer2D( unsigned int _debugMask )
			: m_debugMask( _debugMask )
		{
		}

	public:
		void visit( Node * _node )
		{				
			if( _node->isRenderable() == true && _node->checkVisibility() == true )
			{
				_node->_render( m_debugMask );

				_node->visitChildren( this );
			}
		}

		void visit( Layer * _layer )
		{
			_layer->render( m_debugMask );
		}

	protected:
		unsigned int m_debugMask;
	};
	//////////////////////////////////////////////////////////////////////////
	void Layer2D::render( unsigned int _debugMask )
	{
		Layer::_render( _debugMask );

		Holder<RenderEngine>::hostage()
			->beginLayer2D();

		Camera2D* camera = Holder<Player>::hostage()->getRenderCamera2D();

		mt::vec2f oldPlx = camera->getParallax();

		camera->setParallax( m_factorParallax );

		const mt::mat4f & viewMatrixSecond = camera->getViewMatrix();

		Holder<RenderEngine>::hostage()->setViewMatrix( viewMatrixSecond );

		VisitorRenderLayer2D visitorRender( _debugMask );

		visitChildren( &visitorRender );

		camera->setParallax( oldPlx );

		Holder<RenderEngine>::hostage()
			->endLayer2D();
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer2D::_addChildren( Node * _node )
	{
		_node->setLayer( this );
	}
	//////////////////////////////////////////////////////////////////////////
	mt::vec2f Layer2D::screenToLocal( const mt::vec2f& _point )
	{
		Camera2D* camera = Holder<Player>::hostage()->getRenderCamera2D();
		Viewport vp = camera->getViewport();
		vp.begin.x *= m_factorParallax.x;
		vp.begin.y *= m_factorParallax.y;
		return _point + vp.begin;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Layer2D::testBoundingBox( const Viewport & _viewport, const mt::box2f & _layerspaceBox, const mt::box2f & _screenspaceBox ) const
	{
		Viewport convertView = _viewport;
		convertView.parallax( m_factorParallax );

		bool result = Layer::testBoundingBox( convertView, _layerspaceBox, _screenspaceBox );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	mt::vec2f Layer2D::calcScreenPosition( const Viewport& _viewport, Node* _node ) const
	{
		Viewport vp = _viewport;
		vp.parallax( m_factorParallax );
		mt::vec2f screenPos = _node->getWorldPosition() - vp.begin;
		return screenPos;
	}
	//////////////////////////////////////////////////////////////////////////
}