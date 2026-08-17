#include "Grid2D.h" 

#include "Interface/ResourceServiceInterface.h"

#include "Kernel/ResourceImage.h"
#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ColorHelper.h"

#include "math/box2.h"

#include <cmath>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Grid2D::Grid2D()
        : m_width( 0.f )
        , m_height( 0.f )
        , m_angle( 0.f )
        , m_offset( 0.f, 0.f )
        , m_countX( 0 )
        , m_countY( 0 )
        , m_invalidateVerticesWM( true )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Grid2D::~Grid2D()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void Grid2D::setResourceImage( const ResourceImagePtr & _resourceImage )
    {
        if( m_resourceImage == _resourceImage )
        {
            return;
        }

        this->recompile( [this, &_resourceImage]()
        {
            m_resourceImage = _resourceImage;

            if( m_resourceImage == nullptr )
            {
                return false;
            }

            return true;
        } );
    }
    //////////////////////////////////////////////////////////////////////////
    const ResourceImagePtr & Grid2D::getResourceImage() const
    {
        return m_resourceImage;
    }
    //////////////////////////////////////////////////////////////////////////
    void Grid2D::setOffset( const mt::vec2f & _offset )
    {
        m_offset = _offset;
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & Grid2D::getOffset() const
    {
        return m_offset;
    }
    //////////////////////////////////////////////////////////////////////////
    void Grid2D::setWidth( float _width )
    {
        m_width = _width;
    }
    //////////////////////////////////////////////////////////////////////////
    float Grid2D::getWidth() const
    {
        return m_width;
    }
    //////////////////////////////////////////////////////////////////////////
    void Grid2D::setHeight( float _height )
    {
        m_height = _height;
    }
    //////////////////////////////////////////////////////////////////////////
    float Grid2D::getHeight() const
    {
        return m_height;
    }
    //////////////////////////////////////////////////////////////////////////
    void Grid2D::setCountX( uint32_t _count )
    {
        m_countX = _count;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Grid2D::getCountX() const
    {
        return m_countX;
    }
    //////////////////////////////////////////////////////////////////////////
    void Grid2D::setCountY( uint32_t _count )
    {
        m_countY = _count;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Grid2D::getCountY() const
    {
        return m_countY;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Grid2D::setGridColor( uint32_t _i, uint32_t _j, const Color & _value )
    {
        if( _i >= m_countX || _j >= m_countY )
        {
            return false;
        }

        uint32_t index = _i + _j * m_countX;

        ColorValue_ARGB argb = _value.getAsARGB();

        m_vertices[index].color = argb;

        m_invalidateVerticesWM = true;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Grid2D::getGridColor( uint32_t _i, uint32_t _j, Color * const _value ) const
    {
        if( _i >= m_countX || _j >= m_countY )
        {
            return false;
        }

        uint32_t index = _i + _j * m_countX;

        *_value = Helper::makeColorARGB( m_vertices[index].color );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Grid2D::setGridPosition( uint32_t _i, uint32_t _j, const mt::vec3f & _value )
    {
        if( _i >= m_countX || _j >= m_countY || m_vertices.empty() == true )
        {
            return false;
        }

        m_vertices[_i + _j * m_countX].position = _value;
        m_invalidateVerticesWM = true;
        this->invalidateBoundingBox();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Grid2D::getGridPosition( uint32_t _i, uint32_t _j, mt::vec3f * const _value ) const
    {
        if( _i >= m_countX || _j >= m_countY || m_vertices.empty() == true )
        {
            return false;
        }

        *_value = m_vertices[_i + _j * m_countX].position;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Grid2D::getOriginalGridPosition( uint32_t _i, uint32_t _j, mt::vec3f * const _value ) const
    {
        if( _i >= m_countX || _j >= m_countY || m_originalVertices.empty() == true )
        {
            return false;
        }

        *_value = m_originalVertices[_i + _j * m_countX].position;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Grid2D::resetGrid()
    {
        if( m_originalVertices.empty() == true )
        {
            return;
        }

        for( uint32_t index = 0; index != (uint32_t)m_vertices.size(); ++index )
        {
            m_vertices[index].position = m_originalVertices[index].position;
        }

        m_invalidateVerticesWM = true;
        this->invalidateBoundingBox();
    }
    //////////////////////////////////////////////////////////////////////////
    void Grid2D::applyWaves( float _phase, float _amplitude, float _waves, bool _horizontal, bool _vertical )
    {
        if( m_originalVertices.empty() == true )
        {
            return;
        }

        const float twoPi = 6.28318530717958647692f;

        for( uint32_t index = 0; index != (uint32_t)m_vertices.size(); ++index )
        {
            const mt::vec3f & original = m_originalVertices[index].position;
            mt::vec3f position = original;

            if( _horizontal == true )
            {
                float normalizedY = m_height > 0.f ? original.y / m_height : 0.f;
                position.x += std::sin( normalizedY * _waves * twoPi + _phase ) * _amplitude;
            }

            if( _vertical == true )
            {
                float normalizedX = m_width > 0.f ? original.x / m_width : 0.f;
                position.y += std::sin( normalizedX * _waves * twoPi + _phase ) * _amplitude;
            }

            m_vertices[index].position = position;
        }

        m_invalidateVerticesWM = true;
        this->invalidateBoundingBox();
    }
    //////////////////////////////////////////////////////////////////////////
    void Grid2D::applyRipple( const mt::vec2f & _center, float _radius, float _amplitude, float _waves, float _phase )
    {
        if( m_originalVertices.empty() == true || _radius <= 0.f )
        {
            return;
        }

        const float twoPi = 6.28318530717958647692f;

        for( uint32_t index = 0; index != (uint32_t)m_vertices.size(); ++index )
        {
            const mt::vec3f & original = m_originalVertices[index].position;
            float dx = original.x - _center.x;
            float dy = original.y - _center.y;
            float distance = std::sqrt( dx * dx + dy * dy );
            mt::vec3f position = original;

            if( distance < _radius )
            {
                float normalized = distance / _radius;
                float envelope = 1.f - normalized;
                position.z += std::sin( normalized * _waves * twoPi + _phase ) * _amplitude * envelope;
            }

            m_vertices[index].position = position;
        }

        m_invalidateVerticesWM = true;
        this->invalidateBoundingBox();
    }
    //////////////////////////////////////////////////////////////////////////
    void Grid2D::applyLiquid( float _phase, float _amplitude, float _waves )
    {
        if( m_originalVertices.empty() == true )
        {
            return;
        }

        const float twoPi = 6.28318530717958647692f;

        for( uint32_t index = 0; index != (uint32_t)m_vertices.size(); ++index )
        {
            const mt::vec3f & original = m_originalVertices[index].position;
            float nx = m_width > 0.f ? original.x / m_width : 0.f;
            float ny = m_height > 0.f ? original.y / m_height : 0.f;

            mt::vec3f position = original;
            position.x += std::sin( ny * _waves * twoPi + _phase ) * _amplitude;
            position.y += std::sin( nx * _waves * twoPi + _phase * 1.31f ) * _amplitude;

            m_vertices[index].position = position;
        }

        m_invalidateVerticesWM = true;
        this->invalidateBoundingBox();
    }
    //////////////////////////////////////////////////////////////////////////
    void Grid2D::applyShaky( float _range, uint32_t _seed )
    {
        if( m_originalVertices.empty() == true )
        {
            return;
        }

        uint32_t random = _seed != 0 ? _seed : 1;

        for( uint32_t index = 0; index != (uint32_t)m_vertices.size(); ++index )
        {
            random = random * 1664525U + 1013904223U;
            float x = (float)(random & 0xFFFFU) / 32767.5f - 1.f;
            random = random * 1664525U + 1013904223U;
            float y = (float)(random & 0xFFFFU) / 32767.5f - 1.f;

            mt::vec3f position = m_originalVertices[index].position;
            position.x += x * _range;
            position.y += y * _range;
            m_vertices[index].position = position;
        }

        m_invalidateVerticesWM = true;
        this->invalidateBoundingBox();
    }
    //////////////////////////////////////////////////////////////////////////
    void Grid2D::applyShuffle( float _progress, uint32_t _seed )
    {
        if( m_originalVertices.empty() == true )
        {
            return;
        }

        float progress = mt::clamp( 0.f, _progress, 1.f );
        uint32_t count = (uint32_t)m_vertices.size();
        Vector<uint32_t> order( count );

        for( uint32_t index = 0; index != count; ++index )
        {
            order[index] = index;
        }

        uint32_t random = _seed != 0 ? _seed : 1;

        for( uint32_t index = count; index > 1; --index )
        {
            random = random * 1664525U + 1013904223U;
            uint32_t swapIndex = random % index;
            uint32_t value = order[index - 1];
            order[index - 1] = order[swapIndex];
            order[swapIndex] = value;
        }

        for( uint32_t index = 0; index != count; ++index )
        {
            const mt::vec3f & from = m_originalVertices[index].position;
            const mt::vec3f & to = m_originalVertices[order[index]].position;
            mt::linerp_v3( &m_vertices[index].position, from, to, progress );
        }

        m_invalidateVerticesWM = true;
        this->invalidateBoundingBox();
    }
    //////////////////////////////////////////////////////////////////////////
    void Grid2D::applySplit( float _progress, float _distance, bool _horizontal )
    {
        if( m_originalVertices.empty() == true )
        {
            return;
        }

        float progress = mt::clamp( 0.f, _progress, 1.f );

        for( uint32_t index = 0; index != (uint32_t)m_vertices.size(); ++index )
        {
            mt::vec3f position = m_originalVertices[index].position;

            if( _horizontal == true )
            {
                position.x += (position.y < m_height * 0.5f ? -1.f : 1.f) * _distance * progress;
            }
            else
            {
                position.y += (position.x < m_width * 0.5f ? -1.f : 1.f) * _distance * progress;
            }

            m_vertices[index].position = position;
        }

        m_invalidateVerticesWM = true;
        this->invalidateBoundingBox();
    }
    //////////////////////////////////////////////////////////////////////////
    void Grid2D::applyPageTurn( float _progress, float _radius )
    {
        if( m_originalVertices.empty() == true || _radius <= 0.f )
        {
            return;
        }

        float progress = mt::clamp( 0.f, _progress, 1.f );
        float fold = m_width * (1.f - progress);
        const float halfPi = 1.57079632679489661923f;

        for( uint32_t index = 0; index != (uint32_t)m_vertices.size(); ++index )
        {
            const mt::vec3f & original = m_originalVertices[index].position;
            mt::vec3f position = original;
            float distance = original.x - fold;

            if( distance > 0.f )
            {
                float angle = mt::clamp( 0.f, distance / _radius, halfPi );
                position.x = fold + std::sin( angle ) * _radius;
                position.z = (1.f - std::cos( angle )) * _radius;
            }

            m_vertices[index].position = position;
        }

        m_invalidateVerticesWM = true;
        this->invalidateBoundingBox();
    }
    //////////////////////////////////////////////////////////////////////////
    bool Grid2D::_compile()
    {
        if( m_resourceImage != nullptr )
        {
            if( m_resourceImage->compile() == false )
            {
                LOGGER_ERROR( "grid2d '%s' image resource '%s' not compile"
                    , this->getName().c_str()
                    , m_resourceImage->getName().c_str()
                );

                return false;
            }
        }

        if( m_countX < 2 || m_countY < 2 )
        {
            LOGGER_ERROR( "grid2d '%s' count X|Y not setup %u:%u"
                , this->getName().c_str()
                , m_countX
                , m_countY
            );

            return false;
        }

        m_vertices.resize( m_countX * m_countY );
        m_verticesWM.resize( m_countX * m_countY );

        VectorRenderVertex2D::iterator vertices_iterator = m_vertices.begin();

        float pos_step_x = m_width / float( m_countX - 1 );
        float pos_step_y = m_height / float( m_countY - 1 );

        float pos_uv_x = 1.f / float( m_countX - 1 );
        float pos_uv_y = 1.f / float( m_countY - 1 );

        for( uint32_t j = 0; j != m_countY; ++j )
        {
            for( uint32_t i = 0; i != m_countX; ++i )
            {
                RenderVertex2D & vertex = *vertices_iterator++;

                vertex.position.x = pos_step_x * i;
                vertex.position.y = pos_step_y * j;
                vertex.position.z = 0.f;

                vertex.color = Detail::COLOR_IDENTITY_VALUE;

                vertex.uv[0].x = m_offset.x + pos_uv_x * i;
                vertex.uv[0].y = m_offset.y + pos_uv_y * j;
                vertex.uv[1].x = m_offset.x + pos_uv_x * i;
                vertex.uv[1].y = m_offset.y + pos_uv_y * j;
            }
        }

        m_originalVertices = m_vertices;

        m_indices.resize( (m_countX - 1) * (m_countY - 1) * 6 );

        VectorRenderIndex::iterator indices_iterator = m_indices.begin();

        for( uint32_t j = 0; j != (m_countY - 1); ++j )
        {
            for( uint32_t i = 0; i != (m_countX - 1); ++i )
            {
                uint32_t i0 = (i + 0) + (j + 0) * m_countX;
                uint32_t i1 = (i + 1) + (j + 0) * m_countX;
                uint32_t i2 = (i + 0) + (j + 1) * m_countX;
                uint32_t i3 = (i + 1) + (j + 1) * m_countX;

                *indices_iterator++ = (RenderIndex)i0;
                *indices_iterator++ = (RenderIndex)i2;
                *indices_iterator++ = (RenderIndex)i1;
                *indices_iterator++ = (RenderIndex)i1;
                *indices_iterator++ = (RenderIndex)i2;
                *indices_iterator++ = (RenderIndex)i3;
            }
        }

        m_invalidateVerticesWM = true;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Grid2D::_release()
    {
        if( m_resourceImage != nullptr )
        {
            m_resourceImage->release();
        }
        m_vertices.clear();
        m_originalVertices.clear();
        m_verticesWM.clear();
        m_indices.clear();

        this->releaseMaterial();
    }
    //////////////////////////////////////////////////////////////////////////
    void Grid2D::_dispose()
    {
        m_resourceImage = nullptr;

        Node::_dispose();
    }
    //////////////////////////////////////////////////////////////////////////
    RenderMaterialInterfacePtr Grid2D::_updateMaterial() const
    {
        RenderMaterialInterfacePtr material = this->makeImageMaterial( m_resourceImage, false, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( material, "'%s' m_material is nullptr"
            , this->getName().c_str()
        );

        return material;
    }
    //////////////////////////////////////////////////////////////////////////
    void Grid2D::render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const
    {
        const RenderIndex * indices = m_indices.data();
        uint32_t indicesCount = (uint32_t)m_indices.size();

        uint32_t verticesCount = (uint32_t)m_verticesWM.size();
        const RenderVertex2D * vertices = this->getVerticesWM();

        const RenderMaterialInterfacePtr & material = this->getMaterial();

        const mt::box2f * bb = this->getBoundingBox();

        _renderPipeline->addRenderObject( _context, material, nullptr, vertices, verticesCount, indices, indicesCount, bb, false, MENGINE_DOCUMENT_FORWARD );
    }
    //////////////////////////////////////////////////////////////////////////
    void Grid2D::updateVerticesWM_() const
    {
        m_invalidateVerticesWM = false;

        VectorRenderVertex2D::const_iterator it = m_vertices.begin();
        VectorRenderVertex2D::const_iterator it_end = m_vertices.end();

        VectorRenderVertex2D::iterator it_w = m_verticesWM.begin();

        Color color;
        this->calcTotalColor( &color );

        if( m_resourceImage != nullptr )
        {
            const Color & textureColor = m_resourceImage->getColor();
            color *= textureColor;
        }

        const mt::mat4f & wm = this->getWorldMatrix();

        for( ; it != it_end; ++it, ++it_w )
        {
            const RenderVertex2D & vertex = *it;

            RenderVertex2D & vertex_w = *it_w;

            mt::mul_v3_v3_m4( &vertex_w.position, vertex.position, wm );

            Color vertex_color( color );
            vertex_color *= Helper::makeColorARGB( vertex.color );

            ColorValue_ARGB argb = vertex_color.getAsARGB();

            vertex_w.color = argb;
            vertex_w.uv[0] = vertex.uv[0];
            vertex_w.uv[1] = vertex.uv[1];
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Grid2D::_invalidateWorldMatrix() const
    {
        m_invalidateVerticesWM = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Grid2D::_updateBoundingBox( mt::box2f * const _boundingBox, mt::box2f ** const _boundingBoxCurrent ) const
    {
        const mt::mat4f & wm = this->getWorldMatrix();

        if( m_vertices.empty() == true )
        {
            mt::box2_reset( _boundingBox, 0.f, 0.f );

            *_boundingBoxCurrent = _boundingBox;

            return;
        }

        mt::vec2f worldPoint;
        mt::mul_v2_v2_m4( &worldPoint, mt::vec2f( m_vertices.front().position.x, m_vertices.front().position.y ), wm );
        mt::box2_reset( _boundingBox, worldPoint.x, worldPoint.y );

        for( VectorRenderVertex2D::const_iterator it = m_vertices.begin() + 1; it != m_vertices.end(); ++it )
        {
            const RenderVertex2D & vertex = *it;

            mt::mul_v2_v2_m4( &worldPoint, mt::vec2f( vertex.position.x, vertex.position.y ), wm );
            mt::box2_add_internal_point( _boundingBox, worldPoint.x, worldPoint.y );
        }

        *_boundingBoxCurrent = _boundingBox;
    }
    //////////////////////////////////////////////////////////////////////////
}
