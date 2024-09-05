#include "ArrowInputMousePositionProvider.h"

#include "Interface/TransformationInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ArrowInputMousePositionProvider::ArrowInputMousePositionProvider( const ArrowInterfacePtr & _arrow )
        : m_arrow( _arrow )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ArrowInputMousePositionProvider::~ArrowInputMousePositionProvider()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void ArrowInputMousePositionProvider::onMousePositionChange( ETouchCode _touchId, const mt::vec2f & _position, float _pressure )
    {
        MENGINE_UNUSED( _pressure );

        if( _touchId != TC_TOUCH0 )
        {
            return;
        }

        const NodePtr & node = m_arrow->getNode();

        if( node == nullptr )
        {
            return;
        }

        RenderInterface * render = node->getRender();

        RenderContext context;
        render->makeRenderContext( &context );

        mt::vec2f wp;
        m_arrow->calcMouseWorldPosition( &context, _position, &wp );

        mt::vec3f v3( wp.x, wp.y, 0.f );

        TransformationInterface * transformation = node->getTransformation();

        transformation->setLocalPosition( v3 );
    }
    //////////////////////////////////////////////////////////////////////////
}
