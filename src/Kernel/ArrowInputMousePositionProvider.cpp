#include "ArrowInputMousePositionProvider.h"

#include "Interface/TransformationInterface.h"

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

        RenderContext context;
        render->makeRenderContext( &context );

        mt::vec2f wp;
        m_arrow->calcMouseWorldPosition( &context, _position, &wp );

        mt::vec3f v3( wp.x, wp.y, 0.f );

        TransformationInterface * transformation = m_arrow->getTransformation();

        transformation->setLocalPosition( v3 );
    }
    //////////////////////////////////////////////////////////////////////////
}
