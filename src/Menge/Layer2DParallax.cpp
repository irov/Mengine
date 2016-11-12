#	include "Layer2DParallax.h"

#	include "Kernel/Scene.h"

#	include "Interface/NodeInterface.h"
#   include "Interface/RenderSystemInterface.h"

#	include "Logger/Logger.h"

#	include "Consts.h"

#	include <math.h>

namespace	Menge
{
	//////////////////////////////////////////////////////////////////////////
	Layer2DParallax::Layer2DParallax()
		: m_parallax(1.f, 1.f)
		, m_parallaxLoop( false )
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
	void Layer2DParallax::setParallaxLoop( bool _loop )
	{
		m_parallaxLoop = _loop;
	}
	//////////////////////////////////////////////////////////////////////////
	bool Layer2DParallax::getParallaxLoop() const
	{
		return m_parallaxLoop;
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

		const mt::mat4f & vm = _camera->getCameraViewMatrix();

		mt::vec3f sp;
		mt::mul_m4_v3( sp, vm, wp );

		_screen.x = sp.x;
		_screen.y = sp.y;
	}
	//////////////////////////////////////////////////////////////////////////
	void Layer2DParallax::render( Menge::RenderServiceInterface * _renderService, const RenderObjectState * _state, unsigned int _debugMask )
	{
		const mt::mat4f & camera_vm = _state->camera->getCameraViewMatrix();
		const mt::mat4f & camera_vm_inv = _state->camera->getCameraViewMatrixInv();
		const mt::mat4f & camera_pm = _state->camera->getCameraProjectionMatrix();

		if( m_parallaxLoop == true )
		{
			const mt::vec2f & size = this->getSize();

			float base_x = ::floorf( camera_vm_inv.v3.x * m_parallax.x / size.x );
			float base_y = ::floorf( camera_vm_inv.v3.y * m_parallax.y / size.y );

			for( uint32_t i = 0; i != 4; ++i )
			{
				RenderCameraProxy & rc = m_cameraLoop[i];

				float layer_x = base_x * size.x;
				float layer_y = base_y * size.y;

				layer_x += size.x * float( i % 2 );
				layer_y += size.y * float( i / 2 );

				mt::mat4f vm;
				vm = camera_vm;
				vm.v3.x = (vm.v3.x + layer_x) * m_parallax.x;
				vm.v3.y = (vm.v3.y + layer_y) * m_parallax.y;				

				rc.initialize( camera_pm, vm );

				RenderObjectState state;
				state.viewport = _state->viewport;
				state.camera = &rc;
				state.clipplane = _state->clipplane;
				state.target = _state->target;

				Layer::render( _renderService, &state, _debugMask );
			}
		}
		else
		{
			RenderCameraProxy & rc = m_cameraNorm;

			mt::mat4f vm;
			vm = camera_vm;
			vm.v3.x *= m_parallax.x;
			vm.v3.y *= m_parallax.y;

			rc.initialize( camera_pm, vm );

			RenderObjectState state;
			state.viewport = _state->viewport;
			state.camera = &rc;
			state.clipplane = _state->clipplane;
			state.target = _state->target;

			Layer::render( _renderService, &state, _debugMask );
		}
	}
}
