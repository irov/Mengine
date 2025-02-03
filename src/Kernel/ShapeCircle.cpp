#include "ShapeCircle.h"

#include "Kernel/Surface.h"

#include "Config/StdMath.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ShapeCircle::ShapeCircle()
        : m_quality( 32 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ShapeCircle::~ShapeCircle()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void ShapeCircle::setQuality( uint8_t _quality )
    {
        m_quality = _quality;

        this->invalidateVerticesLocal();
    }
    //////////////////////////////////////////////////////////////////////////
    uint8_t ShapeCircle::getQuality() const
    {
        return m_quality;
    }
    //////////////////////////////////////////////////////////////////////////
    void ShapeCircle::updateVerticesLocal() const
    {
        uint32_t vertexSize = m_quality;
        uint32_t indexSize = (m_quality - 2) * 3;

        m_indices.resize( indexSize );
        m_positions.resize( vertexSize );
        m_verticesWM.resize( vertexSize );


        uint32_t indexIterator = 0;
        uint16_t vertexIterator = 0;

        for( uint16_t index = 0; index != m_quality - 2; ++index )
        {
            m_indices[indexIterator + 0] = 0;
            m_indices[indexIterator + 1] = index + 1;
            m_indices[indexIterator + 2] = index + 2;

            indexIterator += 3;
        }

        const mt::vec2f & size = m_surface->getSize();

        float width = size.x;
        float height = size.y;

        float dt = mt::constant::two_pi / float( m_quality );

        float t = 0.f;

        for( uint32_t index = 0; index != m_quality; ++index )
        {
            float ct = StdMath::cosf( t );
            float st = StdMath::sinf( t );

            float x = width * ct;
            float y = height * st;

            m_positions[vertexIterator].x = x;
            m_positions[vertexIterator].y = y;

            vertexIterator += 1;

            t += dt;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void ShapeCircle::updateVerticesWM() const
    {
        const mt::mat4f & wm = this->getWorldMatrix();

        uint32_t vertexCount = (uint32_t)m_verticesWM.size();

        for( uint32_t i = 0; i != vertexCount; ++i )
        {
            const mt::vec2f & pos = m_positions[i];

            mt::vec3f & wm_pos = m_verticesWM[i].position;
            mt::mul_v3_v2_m4( &wm_pos, pos, wm );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void ShapeCircle::updateVerticesColor() const
    {
        const Mengine::RenderInterface * render = this->getRender();

        Color color;
        render->calcTotalColor( &color );

        const Color & surfaceColor = m_surface->getColor();

        color *= surfaceColor;

        ColorValue_ARGB argb = color.getAsARGB();

        uint32_t vertexCount = (uint32_t)m_verticesWM.size();

        for( uint32_t index = 0; index != vertexCount; ++index )
        {
            m_verticesWM[index].color = argb;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void ShapeCircle::render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const
    {
        uint32_t vertexCount;
        const RenderVertex2D * vertices = this->getVerticesWM( &vertexCount );

        const RenderIndex * indices = m_indices.data();
        uint32_t indexCount = (uint32_t)m_indices.size();

        const RenderMaterialInterfacePtr & material = m_surface->getMaterial();

        const Mengine::RenderInterface * render = this->getRender();

        const mt::box2f * bb = render->getBoundingBox();

        _renderPipeline->addRenderObject( _context, material, nullptr, vertices, vertexCount, indices, indexCount, bb, false, MENGINE_DOCUMENT_FORWARD );
    }
    //////////////////////////////////////////////////////////////////////////
    void ShapeCircle::_updateBoundingBox( mt::box2f * const _boundingBox, mt::box2f ** const _boundingBoxCurrent ) const
    {
        uint32_t vertexCount;
        const RenderVertex2D * vertices = this->getVerticesWM( &vertexCount );

        mt::box2_reset( _boundingBox, vertices[0].position.x, vertices[0].position.y );

        for( uint32_t index = 1; index != vertexCount; ++index )
        {
            const RenderVertex2D & v = vertices[index];

            mt::box2_add_internal_point( _boundingBox, v.position.x, v.position.y );
        }

        *_boundingBoxCurrent = _boundingBox;
    }
    //////////////////////////////////////////////////////////////////////////
}
