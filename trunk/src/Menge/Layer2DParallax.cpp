#	include "Layer2DParallax.h"

#	include "Kernel/Scene.h"

#	include "Camera2D.h"

#	include "Interface/NodeInterface.h"
#   include "Interface/RenderSystemInterface.h"
#   include "Interface/PhysicSystem2DInterface.h"

#	include "Logger/Logger.h"

#	include "Consts.h"

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	Layer2DParallax::Layer2DParallax()
		: m_parallax(1.f, 1.f)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer2DParallax::setParallaxFactor( const mt::vec2f & _parallax )
	{
		m_parallax = _parallax;
	}
	//////////////////////////////////////////////////////////////////////////
	const mt::vec2f & Layer2DParallax::getParallaxFactor() const
	{
		return m_parallax;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Layer2DParallax::_activate()
	{
		Node::_activate();

		if( m_scene == nullptr )
		{
			return false;
		}
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer2DParallax::_deactivate()
	{
		Node::_deactivate();
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer2DParallax::_addChildren( Node * _node )
	{
		_node->setLayer( this );
	}
	//////////////////////////////////////////////////////////////////////////
	mt::vec2f Layer2DParallax::cameraToLocal( Camera2D * _camera2D, const mt::vec2f& _point )
	{
		const Viewport & viewport = _camera2D->getRenderport();

		return _point + viewport.begin;
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer2DParallax::calcScreenPosition( mt::vec2f & _screen, const RenderCameraInterface * _camera, Node * _node ) const
	{
		//Viewport vp = _viewport;
		//Viewport vp = _viewport;
		//vp.parallax( m_factorParallax );

		//const mt::vec3f & wpos = _node->getWorldPosition();

		//_screen = wpos.to_vec2f() - vp.begin;

		const mt::vec3f & wp = _node->getWorldPosition();

		const mt::mat4f & vm = _camera->getViewMatrix();

		mt::vec3f sp;
		mt::mul_m4_v3( sp, vm, wp );

		_screen.x = sp.x;
		_screen.y = sp.y;
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer2DParallax::render( const RenderViewportInterface * _viewport, const RenderCameraInterface * _camera, unsigned int _debugMask )
	{
		m_proxyCamera2D.setup( _camera, m_parallax );

		Layer::render( _viewport, &m_proxyCamera2D, _debugMask );
	}
}
