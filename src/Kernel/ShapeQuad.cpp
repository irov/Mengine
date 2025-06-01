#include "ShapeQuad.h"

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
    void ShapeQuad::render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const
    {
        const RenderVertex2D * vertices = this->getVerticesWM();

        const RenderMaterialInterfacePtr & material = m_surface->getMaterial();

        const Mengine::RenderInterface * render = this->getRender();

        const mt::box2f * bb = render->getBoundingBox();

        _renderPipeline->addRenderQuad( _context, material, vertices, 4, bb, false, MENGINE_DOCUMENT_FORWARD );
    }
    //////////////////////////////////////////////////////////////////////////
    void ShapeQuad::_updateBoundingBox( mt::box2f * const _boundingBox, mt::box2f ** const _boundingBoxCurrent ) const
    {
        const RenderVertex2D * vertices = this->getVerticesWM();

        mt::box2_reset( _boundingBox, vertices[0].position.x, vertices[0].position.y );

        mt::box2_add_internal_point( _boundingBox, vertices[1].position.x, vertices[1].position.y );
        mt::box2_add_internal_point( _boundingBox, vertices[2].position.x, vertices[2].position.y );
        mt::box2_add_internal_point( _boundingBox, vertices[3].position.x, vertices[3].position.y );

        *_boundingBoxCurrent = _boundingBox;
    }
    //////////////////////////////////////////////////////////////////////////
    void ShapeQuad::updateVerticesColor() const
    {
        const Mengine::RenderInterface * render = this->getRender();

        Color color;
        render->calcTotalColor( &color );

        const Color & surfaceColor = m_surface->getColor();

        color *= surfaceColor;

        ColorValue_ARGB argb = color.getAsARGB();

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
            const mt::vec2f & position = m_verticesLocal[i];

            mt::vec3f & wm_position = m_verticesWM[i].position;
            mt::mul_v3_v2_m4( &wm_position, position, wm );
        }
    }
    //////////////////////////////////////////////////////////////////////////
}