#include "Parallax.h"

#include "Interface/RenderViewportInterface.h"
#include "Interface/RenderScissorInterface.h"
#include "Interface/RenderTargetInterface.h"

#include "Logger/Logger.h"

#include <math.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Parallax::Parallax()
        : m_parallaxFactor( 1.f, 1.f, 1.f )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Parallax::~Parallax()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void Parallax::setParallaxFactor( const mt::vec3f & _parallax )
    {
        m_parallaxFactor = _parallax;
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec3f & Parallax::getParallaxFactor() const
    {
        return m_parallaxFactor;
    }
    //////////////////////////////////////////////////////////////////////////
    void Parallax::render( const RenderContext * _state )
    {
        const RenderCameraInterfacePtr & camera = _state->camera;

        const mt::mat4f & camera_vm = camera->getCameraViewMatrix();
        const mt::mat4f & camera_pm = camera->getCameraProjectionMatrix();

        mt::mat4f vm;
        vm = camera_vm;
        vm.v3.x *= m_parallaxFactor.x;
        vm.v3.y *= m_parallaxFactor.y;
        vm.v3.z *= m_parallaxFactor.z;

        m_parallaxCamera.initialize( camera_pm, vm );

        RenderContext state;
        state.viewport = _state->viewport;
        state.camera = &m_parallaxCamera;
        state.scissor = _state->scissor;
        state.target = _state->target;

        Node::render( &state );
    }
}
