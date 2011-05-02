#	include "Layer2D.h"

#	include "BinParser.h"

#	include "Scene.h"

#	include "Camera2D.h"

#	include "Player.h"

#	include "RenderEngine.h"
#	include "PhysicEngine2D.h"

#	include "Logger/Logger.h"
#	include "Game.h"
#	include "Application.h"

#	include "Consts.h"


#	include "Sprite.h"
#	include "Animation.h"

#	include "VisitorAdapter.h"
#	include "NodeManager.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	Layer2D::Layer2D()
		: m_factorParallax(0.f, 0.f)
		, m_renderViewport(0.0f, 0.0f, 0.0f, 0.0f)
		, m_hasViewport(false)
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
	void Layer2D::loader( BinParser * _parser )
	{
		Layer::loader(_parser);

		BIN_SWITCH_ID( _parser )
		{
			BIN_CASE_ATTRIBUTE_METHOD( Protocol::Parallax_Factor, &Layer2D::setParallaxFactor );
			BIN_CASE_ATTRIBUTE_METHOD( Protocol::RenderArea_Value, &Layer2D::setRenderViewport ); //depricated
			BIN_CASE_ATTRIBUTE_METHOD( Protocol::RenderViewport_Value, &Layer2D::setRenderViewport );
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
	void Layer2D::render( Camera2D * _camera )
	{
		RenderEngine::get()
			->beginLayer2D();

		if( m_hasViewport == true )
		{
			RenderPass * pass = RenderEngine::get()
				->getRenderPass();

			RenderEngine::get()
				->setRenderPassViewport( m_renderViewport );

			this->renderChild( _camera );

			RenderEngine::get()
				->setRenderPass( pass );
		}
		else
		{
			this->renderChild( _camera );
		}

		RenderEngine::get()
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
		Camera2D* camera = Player::get()->getRenderCamera2D();
		Viewport vp = camera->getViewport();
		vp.begin.x *= m_factorParallax.x;
		vp.begin.y *= m_factorParallax.y;
		return _point + vp.begin;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Layer2D::testBoundingBox( const Viewport & _viewport, const mt::box2f & _layerspaceBox, const mt::box2f & _screenspaceBox ) const
	{
		Viewport vp = Player::get()->getRenderCamera2D()->getViewport();
		vp.parallax( m_factorParallax );

		bool result = Layer::testBoundingBox( vp, _layerspaceBox, _screenspaceBox );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer2D::calcScreenPosition( mt::vec2f & _screen, const Viewport& _viewport, Node* _node ) const
	{
		//Viewport vp = _viewport;
		Viewport vp = Player::get()->getRenderCamera2D()->getViewport();
		vp.parallax( m_factorParallax );

		_screen = _node->getWorldPosition() - vp.begin;
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer2D::setRenderViewport( const Viewport & _viewport )
	{
		m_renderViewport = _viewport;

		m_hasViewport = true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer2D::removeRenderViewport()
	{
		m_hasViewport = false;
	}
	//////////////////////////////////////////////////////////////////////////
	const Viewport & Layer2D::getRenderViewport() const
	{
		return m_renderViewport;
	}
	//////////////////////////////////////////////////////////////////////////
}
