#include "ArrowInputMousePositionProvider.h"

#include "Kernel/Arrow.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ArrowInputMousePositionProvider::ArrowInputMousePositionProvider( Arrow * _arrow )
        : m_arrow( _arrow )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ArrowInputMousePositionProvider::~ArrowInputMousePositionProvider()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void ArrowInputMousePositionProvider::onMousePositionChange( uint32_t _touchId, const mt::vec2f & _position, float _pressure )
    {
        MENGINE_UNUSED( _pressure );

        if( _touchId != 0 )
        {
            return;
        }

        RenderInterface * render = m_arrow->getRender();

        const RenderCameraInterfacePtr & renderCamera = render->getRenderCamera();
        const RenderViewportInterfacePtr & renderViewport = render->getRenderViewport();

        mt::vec2f wp;
        m_arrow->calcMouseWorldPosition( renderCamera, renderViewport, _position, &wp );

        mt::vec3f v3( wp.x, wp.y, 0.f );

        m_arrow->setLocalPosition( v3 );
    }
    //////////////////////////////////////////////////////////////////////////
}
