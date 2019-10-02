#include "ShapePacMan.h"

#include "Kernel/Surface.h"

#include "Kernel/Logger.h"
#include "Kernel/Document.h"

#include "math/angle.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ShapePacMan::ShapePacMan()
        : m_angleFrom( 0.f )
        , m_angleTo( mt::constant::half_pi / 8.f + mt::constant::fourth_pi * 7.f )
        , m_indexCount( 0 )
        , m_vertexCount( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ShapePacMan::~ShapePacMan()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void ShapePacMan::setAngleFrom( float _angleFrom )
    {
        m_angleFrom = mt::angle_norm( _angleFrom );

        this->invalidateVerticesLocal();
    }
    //////////////////////////////////////////////////////////////////////////
    float ShapePacMan::getAngleFrom() const
    {
        return m_angleFrom;
    }
    //////////////////////////////////////////////////////////////////////////
    void ShapePacMan::setAngleTo( float _angleTo )
    {
        m_angleTo = mt::angle_norm( _angleTo );

        this->invalidateVerticesLocal();
    }
    //////////////////////////////////////////////////////////////////////////
    float ShapePacMan::getAngleTo() const
    {
        return m_angleTo;
    }
    //////////////////////////////////////////////////////////////////////////
    void ShapePacMan::updateVerticesLocal() const
    {
        const mt::vec2f & size = m_surface->getSize();
        const mt::vec2f & offset = m_surface->getOffset();

        if( mt::equal_f_z( m_angleTo ) == true )
        {
            m_verticesLocal[0].x = offset.x + 0.f;
            m_verticesLocal[0].y = offset.y + 0.f;

            m_verticesLocal[1].x = offset.x + size.x;
            m_verticesLocal[1].y = offset.y + 0.f;

            m_verticesLocal[2].x = offset.x + size.x;
            m_verticesLocal[2].y = offset.y + size.y;

            m_verticesLocal[3].x = offset.x + 0.f;
            m_verticesLocal[3].y = offset.y + size.y;

            m_vertexCount = 4;

            m_indices[0 * 3 + 0] = 0;
            m_indices[0 * 3 + 1] = 1;
            m_indices[0 * 3 + 2] = 2;

            m_indices[1 * 3 + 0] = 0;
            m_indices[1 * 3 + 1] = 2;
            m_indices[1 * 3 + 2] = 3;

            m_indexCount = 6;
        }
        else if( m_angleTo < mt::constant::fourth_pi * 1.f )
        {
            m_verticesLocal[0].x = offset.x + size.x * 0.5f;
            m_verticesLocal[0].y = offset.y + size.y * 0.5f;

            m_verticesLocal[1].x = offset.x + 0.f;
            m_verticesLocal[1].y = offset.y + 0.f;

            m_verticesLocal[2].x = offset.x + size.x * 0.5f;
            m_verticesLocal[2].y = offset.y + 0.f;

            m_verticesLocal[3].x = offset.x + size.x * 0.5f + MT_sinf( m_angleTo ) * size.y * 0.5f;
            m_verticesLocal[3].y = offset.y + 0.f;

            m_verticesLocal[4].x = offset.x + size.x;
            m_verticesLocal[4].y = offset.y + 0.f;

            m_verticesLocal[5].x = offset.x + size.x;
            m_verticesLocal[5].y = offset.y + size.y;

            m_verticesLocal[6].x = offset.x + 0.f;
            m_verticesLocal[6].y = offset.y + size.y;

            m_vertexCount = 7;

            m_indices[0 * 3 + 0] = 0;
            m_indices[0 * 3 + 1] = 1;
            m_indices[0 * 3 + 2] = 2;

            m_indices[1 * 3 + 0] = 0;
            m_indices[1 * 3 + 1] = 3;
            m_indices[1 * 3 + 2] = 4;

            m_indices[2 * 3 + 0] = 0;
            m_indices[2 * 3 + 1] = 4;
            m_indices[2 * 3 + 2] = 5;

            m_indices[3 * 3 + 0] = 0;
            m_indices[3 * 3 + 1] = 5;
            m_indices[3 * 3 + 2] = 6;

            m_indices[4 * 3 + 0] = 0;
            m_indices[4 * 3 + 1] = 6;
            m_indices[4 * 3 + 2] = 1;

            m_indexCount = 16;
        }
        else if( m_angleTo < mt::constant::fourth_pi * 2.f )
        {
            m_verticesLocal[0].x = offset.x + size.x * 0.5f;
            m_verticesLocal[0].y = offset.y + size.y * 0.5f;

            m_verticesLocal[1].x = offset.x + 0.f;
            m_verticesLocal[1].y = offset.y + 0.f;

            m_verticesLocal[2].x = offset.x + size.x * 0.5f;
            m_verticesLocal[2].y = offset.y + 0.f;

            m_verticesLocal[3].x = offset.x + size.x;
            m_verticesLocal[3].y = offset.y + 0.f;

            m_verticesLocal[4].x = offset.x + size.x;
            m_verticesLocal[4].y = offset.y + size.y * 0.5f - MT_sinf( mt::constant::half_pi - m_angleTo ) * size.x * 0.5f;

            m_verticesLocal[5].x = offset.x + size.x;
            m_verticesLocal[5].y = offset.y + size.y;

            m_verticesLocal[6].x = offset.x + 0.f;
            m_verticesLocal[6].y = offset.y + size.y;

            m_vertexCount = 7;

            m_indices[0 * 3 + 0] = 0;
            m_indices[0 * 3 + 1] = 1;
            m_indices[0 * 3 + 2] = 2;

            m_indices[1 * 3 + 0] = 0;
            m_indices[1 * 3 + 1] = 4;
            m_indices[1 * 3 + 2] = 5;

            m_indices[2 * 3 + 0] = 0;
            m_indices[2 * 3 + 1] = 5;
            m_indices[2 * 3 + 2] = 6;

            m_indices[3 * 3 + 0] = 0;
            m_indices[3 * 3 + 1] = 6;
            m_indices[3 * 3 + 2] = 1;

            m_indexCount = 12;
        }
        else if( m_angleTo < mt::constant::fourth_pi * 3.f )
        {
            m_verticesLocal[0].x = offset.x + size.x * 0.5f;
            m_verticesLocal[0].y = offset.y + size.y * 0.5f;

            m_verticesLocal[1].x = offset.x + 0.f;
            m_verticesLocal[1].y = offset.y + 0.f;

            m_verticesLocal[2].x = offset.x + size.x * 0.5f;
            m_verticesLocal[2].y = offset.y + 0.f;

            m_verticesLocal[3].x = offset.x + size.x;
            m_verticesLocal[3].y = offset.y + 0.f;

            m_verticesLocal[4].x = offset.x + size.x;
            m_verticesLocal[4].y = offset.y + size.y * 0.5f + MT_sinf( m_angleTo - mt::constant::half_pi ) * size.x * 0.5f;

            m_verticesLocal[5].x = offset.x + size.x;
            m_verticesLocal[5].y = offset.y + size.y;

            m_verticesLocal[6].x = offset.x + 0.f;
            m_verticesLocal[6].y = offset.y + size.y;

            m_vertexCount = 7;

            m_indices[0 * 3 + 0] = 0;
            m_indices[0 * 3 + 1] = 1;
            m_indices[0 * 3 + 2] = 2;

            m_indices[1 * 3 + 0] = 0;
            m_indices[1 * 3 + 1] = 4;
            m_indices[1 * 3 + 2] = 5;

            m_indices[2 * 3 + 0] = 0;
            m_indices[2 * 3 + 1] = 5;
            m_indices[2 * 3 + 2] = 6;

            m_indices[3 * 3 + 0] = 0;
            m_indices[3 * 3 + 1] = 6;
            m_indices[3 * 3 + 2] = 1;

            m_indexCount = 12;
        }
        else if( m_angleTo < mt::constant::fourth_pi * 4.f )
        {
            m_verticesLocal[0].x = offset.x + size.x * 0.5f;
            m_verticesLocal[0].y = offset.y + size.y * 0.5f;

            m_verticesLocal[1].x = offset.x + 0.f;
            m_verticesLocal[1].y = offset.y + 0.f;

            m_verticesLocal[2].x = offset.x + size.x * 0.5f;
            m_verticesLocal[2].y = offset.y + 0.f;

            m_verticesLocal[3].x = offset.x + size.x;
            m_verticesLocal[3].y = offset.y + 0.f;

            m_verticesLocal[4].x = offset.x + size.x;
            m_verticesLocal[4].y = offset.y + size.y;

            m_verticesLocal[5].x = offset.x + size.x * 0.5f + MT_sinf( mt::constant::pi - m_angleTo ) * size.y * 0.5f;
            m_verticesLocal[5].y = offset.y + size.y;

            m_verticesLocal[6].x = offset.x + 0.f;
            m_verticesLocal[6].y = offset.y + size.y;

            m_vertexCount = 7;

            m_indices[0 * 3 + 0] = 0;
            m_indices[0 * 3 + 1] = 1;
            m_indices[0 * 3 + 2] = 2;

            m_indices[1 * 3 + 0] = 0;
            m_indices[1 * 3 + 1] = 5;
            m_indices[1 * 3 + 2] = 6;

            m_indices[2 * 3 + 0] = 0;
            m_indices[2 * 3 + 1] = 6;
            m_indices[2 * 3 + 2] = 1;

            m_indexCount = 9;
        }
        else if( m_angleTo < mt::constant::fourth_pi * 5.f )
        {
            m_verticesLocal[0].x = offset.x + size.x * 0.5f;
            m_verticesLocal[0].y = offset.y + size.y * 0.5f;

            m_verticesLocal[1].x = offset.x + 0.f;
            m_verticesLocal[1].y = offset.y + 0.f;

            m_verticesLocal[2].x = offset.x + size.x * 0.5f;
            m_verticesLocal[2].y = offset.y + 0.f;

            m_verticesLocal[3].x = offset.x + size.x;
            m_verticesLocal[3].y = offset.y + 0.f;

            m_verticesLocal[4].x = offset.x + size.x;
            m_verticesLocal[4].y = offset.y + size.y;

            m_verticesLocal[5].x = offset.x + size.x * 0.5f - MT_sinf( m_angleTo - mt::constant::pi ) * size.y * 0.5f;
            m_verticesLocal[5].y = offset.y + size.y;

            m_verticesLocal[6].x = offset.x + 0.f;
            m_verticesLocal[6].y = offset.y + size.y;

            m_vertexCount = 7;

            m_indices[0 * 3 + 0] = 0;
            m_indices[0 * 3 + 1] = 1;
            m_indices[0 * 3 + 2] = 2;

            m_indices[1 * 3 + 0] = 0;
            m_indices[1 * 3 + 1] = 5;
            m_indices[1 * 3 + 2] = 6;

            m_indices[2 * 3 + 0] = 0;
            m_indices[2 * 3 + 1] = 6;
            m_indices[2 * 3 + 2] = 1;

            m_indexCount = 9;
        }
        else if( m_angleTo < mt::constant::fourth_pi * 6.f )
        {
            m_verticesLocal[0].x = offset.x + size.x * 0.5f;
            m_verticesLocal[0].y = offset.y + size.y * 0.5f;

            m_verticesLocal[1].x = offset.x + 0.f;
            m_verticesLocal[1].y = offset.y + 0.f;

            m_verticesLocal[2].x = offset.x + size.x * 0.5f;
            m_verticesLocal[2].y = offset.y + 0.f;

            m_verticesLocal[3].x = offset.x + size.x;
            m_verticesLocal[3].y = offset.y + 0.f;

            m_verticesLocal[4].x = offset.x + size.x;
            m_verticesLocal[4].y = offset.y + size.y;

            m_verticesLocal[5].x = offset.x + 0.f;
            m_verticesLocal[5].y = offset.y + size.y;

            m_verticesLocal[6].x = offset.x + 0.f;
            m_verticesLocal[6].y = offset.y + size.y * 0.5f + MT_sinf( mt::constant::pi + mt::constant::half_pi - m_angleTo ) * size.x * 0.5f;

            m_vertexCount = 7;

            m_indices[0 * 3 + 0] = 0;
            m_indices[0 * 3 + 1] = 1;
            m_indices[0 * 3 + 2] = 2;

            m_indices[1 * 3 + 0] = 0;
            m_indices[1 * 3 + 1] = 6;
            m_indices[1 * 3 + 2] = 1;

            m_indexCount = 6;
        }
        else if( m_angleTo < mt::constant::fourth_pi * 7.f )
        {
            m_verticesLocal[0].x = offset.x + size.x * 0.5f;
            m_verticesLocal[0].y = offset.y + size.y * 0.5f;

            m_verticesLocal[1].x = offset.x + 0.f;
            m_verticesLocal[1].y = offset.y + 0.f;

            m_verticesLocal[2].x = offset.x + size.x * 0.5f;
            m_verticesLocal[2].y = offset.y + 0.f;

            m_verticesLocal[3].x = offset.x + size.x;
            m_verticesLocal[3].y = offset.y + 0.f;

            m_verticesLocal[4].x = offset.x + size.x;
            m_verticesLocal[4].y = offset.y + size.y;

            m_verticesLocal[5].x = offset.x + 0.f;
            m_verticesLocal[5].y = offset.y + size.y;

            m_verticesLocal[6].x = offset.x + 0.f;
            m_verticesLocal[6].y = offset.y + size.y * 0.5f - MT_sinf( m_angleTo - mt::constant::pi - mt::constant::half_pi ) * size.x * 0.5f;

            m_vertexCount = 7;

            m_indices[0 * 3 + 0] = 0;
            m_indices[0 * 3 + 1] = 1;
            m_indices[0 * 3 + 2] = 2;

            m_indices[1 * 3 + 0] = 0;
            m_indices[1 * 3 + 1] = 6;
            m_indices[1 * 3 + 2] = 1;

            m_indexCount = 6;
        }
        else if( m_angleTo < mt::constant::fourth_pi * 8.f )
        {
            m_verticesLocal[0].x = offset.x + size.x * 0.5f;
            m_verticesLocal[0].y = offset.y + size.y * 0.5f;

            m_verticesLocal[1].x = offset.x + 0.f;
            m_verticesLocal[1].y = offset.y + 0.f;

            m_verticesLocal[2].x = offset.x + size.x * 0.5f;
            m_verticesLocal[2].y = offset.y + 0.f;

            m_verticesLocal[3].x = offset.x + size.x;
            m_verticesLocal[3].y = offset.y + 0.f;

            m_verticesLocal[4].x = offset.x + size.x;
            m_verticesLocal[4].y = offset.y + size.y;

            m_verticesLocal[5].x = offset.x + 0.f;
            m_verticesLocal[5].y = offset.y + size.y;

            m_verticesLocal[6].x = offset.x + size.x * 0.5f - MT_sinf( mt::constant::two_pi - m_angleTo ) * size.y * 0.5f;
            m_verticesLocal[6].y = offset.y + 0.f;

            m_vertexCount = 7;

            m_indices[0 * 3 + 0] = 0;
            m_indices[0 * 3 + 1] = 6;
            m_indices[0 * 3 + 2] = 2;

            m_indexCount = 3;
        }
        else if( mt::equal_f_f( m_angleTo, mt::constant::two_pi ) == true )
        {
            m_vertexCount = 0;
            m_indexCount = 0;
        }

        uint32_t uvCount = m_surface->getUVCount();

        for( uint32_t indexUV = 0; indexUV != uvCount; ++indexUV )
        {
            const mt::uv4f & uv = m_surface->getUV( indexUV );

            for( uint32_t indexVertex = 0; indexVertex != m_vertexCount; ++indexVertex )
            {
                RenderVertex2D & v = m_verticesWM[indexVertex];

                mt::vec2f p = v.position.to_vec2f() / size;

                mt::uv4_quad_point( v.uv[indexUV], uv, p );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void ShapePacMan::render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const
    {
        const RenderVertex2D * vertices = this->getVerticesWM();

        const RenderMaterialInterfacePtr & material = m_surface->getMaterial();

        const mt::box2f * bb = this->getBoundingBox();

        _renderPipeline->addRenderObject( _context, material, nullptr, vertices, m_vertexCount, m_indices, m_indexCount, bb, false, MENGINE_DOCUMENT_FUNCTION );
    }
    //////////////////////////////////////////////////////////////////////////
    void ShapePacMan::_updateBoundingBox( mt::box2f & _boundingBox, mt::box2f ** _boundingBoxCurrent ) const
    {
        const RenderVertex2D * vertices = this->getVerticesWM();

        mt::reset( _boundingBox, vertices[0].position.x, vertices[0].position.y );

        for( uint32_t index = 1; index != m_vertexCount; ++index )
        {
            const RenderVertex2D & v = vertices[index];

            mt::add_internal_point( _boundingBox, v.position.x, v.position.y );
        }

        *_boundingBoxCurrent = &_boundingBox;
    }
    //////////////////////////////////////////////////////////////////////////
    void ShapePacMan::updateVerticesColor() const
    {
        Color color;
        this->calcTotalColor( color );

        const Color & surfaceColor = m_surface->getColor();

        color *= surfaceColor;

        uint32_t argb = color.getAsARGB();

        for( uint32_t index = 0; index != 8; ++index )
        {
            m_verticesWM[index].color = argb;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void ShapePacMan::updateVerticesWM() const
    {
        const mt::mat4f & wm = this->getWorldMatrix();

        for( uint32_t i = 0; i != m_vertexCount; ++i )
        {
            const mt::vec2f & pos = m_verticesLocal[i];

            mt::vec3f & wm_pos = m_verticesWM[i].position;
            mt::mul_v3_v2_m4( wm_pos, pos, wm );
        }
    }
}
