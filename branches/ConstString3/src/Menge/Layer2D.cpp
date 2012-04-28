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
		, m_viewport(0.f, 0.f, 0.f, 0.f)
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
			mt::mat4f old_vm;
			mt::mat4f old_pm;
			
			RenderEngine::get()
				->getCurrentRenderPass( old_vp, old_vm, old_pm );

			RenderEngine::get()
				->newRenderPass( m_viewportWM, m_viewMatrix, m_projectionMatrix );

			this->renderChild( _camera );

			RenderEngine::get()
				->newRenderPass( old_vp, old_vm, old_pm );
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

		const mt::vec3f & wpos = _node->getWorldPosition();

		_screen = wpos.to_vec2f() - vp.begin;
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer2D::setRenderViewport( const Viewport & _viewport )
	{
		m_viewport = _viewport;
		
		m_hasViewport = true;

		this->invalidateWorldMatrix();
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer2D::removeRenderViewport()
	{
		m_hasViewport = false;
	}
	//////////////////////////////////////////////////////////////////////////
	const Viewport & Layer2D::getRenderViewport() const
	{
		return m_viewport;
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer2D::_invalidateWorldMatrix()
	{
		Node::_invalidateWorldMatrix();

		if( m_hasViewport == true )
		{			
			const mt::mat4f & wm = this->getWorldMatrix();

			mt::mul_v2_m4( m_viewportWM.begin, m_viewport.begin, wm );
			mt::mul_v2_m4( m_viewportWM.end, m_viewport.end, wm );
			
			RenderEngine::get()
				->makeViewMatrixFromViewport(m_viewMatrix, m_viewportWM);

			RenderEngine::get()
				->makeProjectionOrthogonalFromViewport(m_projectionMatrix, m_viewportWM);
		}
	}
}
