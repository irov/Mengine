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
			XML_CASE_ATTRIBUTE_NODE_METHOD( "Parallax", "Factor", &Layer2D::setParallaxFactor );
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
}