#include "ShapeQuad.h"

#include "Interface/RenderSystemInterface.h"

#include "Kernel/Surface.h"

#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ShapeQuad::ShapeQuad()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ShapeQuad::~ShapeQuad()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void ShapeQuad::_render( const RenderContext * _state )
    {
        const RenderVertex2D * vertices = this->getVerticesWM();

        const RenderMaterialInterfacePtr & material = m_surface->getMaterial();

        const mt::box2f & bb = this->getBoundingBox();

        this->addRenderQuad( _state, material, vertices, 4, &bb, false );
    }
    //////////////////////////////////////////////////////////////////////////
    void ShapeQuad::_updateBoundingBox( mt::box2f & _boundingBox ) const
    {
        const RenderVertex2D * vertices = this->getVerticesWM();

        mt::reset( _boundingBox, vertices[0].position.x, vertices[0].position.y );

        mt::add_internal_point( _boundingBox, vertices[1].position.x, vertices[1].position.y );
        mt::add_internal_point( _boundingBox, vertices[2].position.x, vertices[2].position.y );
        mt::add_internal_point( _boundingBox, vertices[3].position.x, vertices[3].position.y );
    }
    //////////////////////////////////////////////////////////////////////////
    void ShapeQuad::updateVerticesColor() const
    {
        Color color;
        this->calcTotalColor( color );

        const Color & surfaceColor = m_surface->getColor();

        color *= surfaceColor;

        uint32_t argb = color.getAsARGB();

        m_verticesWM[0].color = argb;
        m_verticesWM[1].color = argb;
        m_verticesWM[2].color = argb;
        m_verticesWM[3].color = argb;
    }
    //////////////////////////////////////////////////////////////////////////
    void ShapeQuad::updateVerticesWM() const
    {
        const mt::mat4f & wm = this->getWorldMatrix();

        for( uint32_t i = 0; i != 4; ++i )
        {
            const mt::vec2f & pos = m_verticesLocal[i];

            mt::vec3f & wm_pos = m_verticesWM[i].position;
            mt::mul_v3_v2_m4( wm_pos, pos, wm );
        }
    }
}