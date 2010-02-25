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
#	include "SceneManager.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	FACTORABLE_IMPLEMENT(Layer2D);
	//////////////////////////////////////////////////////////////////////////
	Layer2D::Layer2D()
		: m_factorParallax(0.f,0.f)
		, m_camera2D( NULL )
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer2D::setParallaxFactor( const mt::vec2f & _factor )
	{
		m_factorParallax = _factor;
		if( m_camera2D != NULL )
		{
			m_camera2D->setParallax( m_factorParallax );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & Layer2D::getParallaxFactor() const
	{
		return m_factorParallax;
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

		const Resolution& res = Game::hostage()
									->getContentResolution();

		m_camera2D = Holder<SceneManager>::hostage()->createNodeT<Camera2D>( "Camera2D" );
		m_camera2D->setViewportSize( mt::vec2f( res[0], res[1] ) );

		m_scene->getCamera()
			->addChildren( m_camera2D );

		m_camera2D->setParallax( m_factorParallax );
		m_camera2D->setRenderViewport( m_renderViewport );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer2D::_deactivate()
	{
		if( m_camera2D != NULL )
		{
			//m_scene->getCamera()
			//	->removeChildren( m_camera2D );

			m_camera2D->destroy();
			m_camera2D = NULL;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	//class VisitorRenderLayer2D
	//	: public VisitorAdapter<VisitorRenderLayer2D>
	//{
	//public:
	//	VisitorRenderLayer2D( unsigned int _debugMask )
	//		: m_debugMask( _debugMask )
	//	{
	//	}

	//public:
	//	void visit( Node * _node )
	//	{			
	//		_node->render( m_debugMask );
	//		//if( _node->isRenderable() == false )
	//		//{
	//		//	return;
	//		//}

	//		//if( _node->checkVisibility() == false )
	//		//{
	//		//	return;
	//		//}

	//		//_node->_render( m_debugMask );
	//		//_node->visitChildren( this );
	//	}

	//	void visit( Layer * _layer )
	//	{
	//		_layer->render( m_debugMask );
	//	}

	//protected:
	//	unsigned int m_debugMask;
	//};
	//////////////////////////////////////////////////////////////////////////
	void Layer2D::render( Camera2D * _camera )
	{
		//Layer::_render( _debugMask );

		Holder<RenderEngine>::hostage()
			->beginLayer2D();

		//Camera2D* camera = Holder<Player>::hostage()->getRenderCamera2D();

		//mt::vec2f oldPlx = camera->getParallax();

		//camera->setParallax( m_factorParallax );

		//const mt::mat4f & viewMatrixSecond = camera->getViewMatrix();

		//Holder<RenderEngine>::hostage()->setViewMatrix( viewMatrixSecond );
		Holder<RenderEngine>::hostage()
			->setActiveCamera( m_camera2D );

		//VisitorRenderLayer2D visitorRender( _debugMask );

		this->renderChild( m_camera2D );
		//visitChildren( &visitorRender );

		//camera->setParallax( oldPlx );


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
		Viewport convertView = m_scene->getCamera()->getViewport();
		convertView.parallax( m_factorParallax );

		bool result = Layer::testBoundingBox( convertView, _layerspaceBox, _screenspaceBox );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	mt::vec2f Layer2D::calcScreenPosition( const Viewport& _viewport, Node* _node ) const
	{
		//Viewport vp = _viewport;
		Viewport vp = m_scene->getCamera()->getViewport();
		vp.parallax( m_factorParallax );
		mt::vec2f screenPos = _node->getWorldPosition() - vp.begin;
		return screenPos;
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer2D::setRenderViewport( const Viewport & _viewport )
	{
		Layer::setRenderViewport( _viewport );

		if( m_camera2D != NULL )
		{
			m_camera2D->setRenderViewport( m_renderViewport );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	Camera* Layer2D::getCamera()
	{
		return m_camera2D;
	}
	//////////////////////////////////////////////////////////////////////////
}
