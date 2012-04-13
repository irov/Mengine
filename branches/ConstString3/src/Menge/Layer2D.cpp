#	include "Layer2D.h"

#	include "Scene.h"

#	include "Camera2D.h"

#	include "Player.h"

#	include "RenderEngine.h"
#	include "PhysicEngine2D.h"

#	include "LogEngine.h"
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
		: m_factorParallax(1.f, 1.f)
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
			Viewport old_vp;
			mt::mat3f old_wm;
			
			RenderEngine::get()
				->getCurrentRenderPass( old_vp, old_wm );

			const mt::mat3f & wm = this->getWorldMatrix();

			Viewport new_vp;
			mt::mul_v2_m3( new_vp.begin, m_renderViewport.begin, wm );
			mt::mul_v2_m3( new_vp.end, m_renderViewport.end, wm );

			mt::mat3f camera_wm;
			mt::ident_m3(camera_wm);

			RenderEngine::get()
				->newRenderPass( new_vp, camera_wm );

			this->renderChild( _camera );

			RenderEngine::get()
				->newRenderPass( old_vp, old_wm );
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
	mt::vec2f Layer2D::cameraToLocal( Camera2D * _camera2D, const mt::vec2f& _point )
	{
		const Viewport & viewport = _camera2D->getViewport();

		Viewport vp = viewport;
		vp.begin.x *= m_factorParallax.x;
		vp.begin.y *= m_factorParallax.y;

		return _point + vp.begin;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Layer2D::testBoundingBox( const Viewport & _viewport, const mt::box2f & _layerspaceBox, const mt::box2f & _screenspaceBox ) const
	{
		Viewport vp = _viewport;
		vp.parallax( m_factorParallax );

		bool result = Layer::testBoundingBox( _viewport, _layerspaceBox, _screenspaceBox );

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer2D::calcScreenPosition( mt::vec2f & _screen, const Viewport& _viewport, Node* _node ) const
	{
		//Viewport vp = _viewport;
		Viewport vp = _viewport;
		vp.parallax( m_factorParallax );

		const mt::vec2f & wpos = _node->getWorldPosition();

		_screen = wpos - vp.begin;
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
}
