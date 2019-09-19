#include "Grid2D.h" 

#include "Interface/RenderSystemInterface.h"
#include "Interface/ResourceServiceInterface.h"

#include "Kernel/ResourceImage.h"
#include "Kernel/Logger.h"
#include "Kernel/Document.h"
#include "Kernel/AssertionMemoryPanic.h"

#include "math/box2.h"
#include "math/clamp.h"

#include <math.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    Grid2D::Grid2D()
        : m_width( 0.f )
        , m_height( 0.f )
        , m_countX( 0 )
        , m_countY( 0 )
        , m_invalidateVerticesWM( true )
        , m_offset( 0.f, 0.f )
        , m_angle( 0.f )
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

        this->recompile( [this, _resourceImage]()
        {
            m_resourceImage = _resourceImage;
        } );
    }
    //////////////////////////////////////////////////////////////////////////
    const ResourceImagePtr & Grid2D::getResourceImage() const
    {
        return m_resourceImage;
    }
    //////////////////////////////////////////////////////////////////////////
    void Grid2D::setAngle( const mt::vec2f & _offset, float _angle )
    {
        m_offset = _offset;
        m_angle = _angle;
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
    bool Grid2D::getGridColor( uint32_t _i, uint32_t _j, Color & _value ) const
    {
        if( _i >= m_countX || _j >= m_countY )
        {
            return false;
        }

        uint32_t index = _i + _j * m_countX;

        _value.setAsARGB( m_vertices[index].color );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Grid2D::_compile()
    {
        if( m_resourceImage != nullptr )
        {
            if( m_resourceImage->compile() == false )
            {
                LOGGER_ERROR( "'%s' image resource '%s' not compile"
                    , this->getName().c_str()
                    , m_resourceImage->getName().c_str()
                );

                return false;
            }
        }

        if( m_countX < 2 || m_countY < 2 )
        {
            LOGGER_ERROR( "'%s' count X|Y not setup %d:%d"
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
        m_resourceImage->release();

        m_vertices.clear();
        m_verticesWM.clear();
        m_indices.clear();

        this->releaseMaterial();
    }
    //////////////////////////////////////////////////////////////////////////
    RenderMaterialInterfacePtr Grid2D::_updateMaterial() const
    {
        RenderMaterialInterfacePtr material = this->makeImageMaterial( m_resourceImage, false, MENGINE_DOCUMENT_FUNCTION );

        MENGINE_ASSERTION_MEMORY_PANIC( material, nullptr, "'%s' m_material is NULL"
            , this->getName().c_str()
        );

        return material;
    }
    //////////////////////////////////////////////////////////////////////////
    void Grid2D::render( const RenderContext * _context ) const
    {
        const RenderIndex * indices = &m_indices[0];
        uint32_t indicesCount = (uint32_t)m_indices.size();

        const RenderVertex2D * vertices = this->getVerticesWM();
        const RenderMaterialInterfacePtr & material = this->getMaterial();

        uint32_t verticesCount = (uint32_t)m_verticesWM.size();

        const mt::box2f * bb = this->getBoundingBox();

        this->addRenderObject( _context, material, nullptr, vertices, verticesCount, indices, indicesCount, bb, false, MENGINE_DOCUMENT_FUNCTION );
    }
    //////////////////////////////////////////////////////////////////////////
    void Grid2D::updateVerticesWM_() const
    {
        m_invalidateVerticesWM = false;

        VectorRenderVertex2D::const_iterator it = m_vertices.begin();
        VectorRenderVertex2D::const_iterator it_end = m_vertices.end();

        VectorRenderVertex2D::iterator it_w = m_verticesWM.begin();

        Color color;
        this->calcTotalColor( color );

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

            mt::mul_v3_v3_m4( vertex_w.position, vertex.position, wm );

            Color vertex_color( color );
            vertex_color *= Color( vertex.color );

            uint32_t argb = vertex_color.getAsARGB();

            vertex_w.color = argb;
            vertex_w.uv[0] = vertex.uv[0];
            vertex_w.uv[1] = vertex.uv[1];
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Grid2D::_invalidateWorldMatrix()
    {
        Node::_invalidateWorldMatrix();

        m_invalidateVerticesWM = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Grid2D::_updateBoundingBox( mt::box2f & _boundingBox, mt::box2f ** _boundingBoxCurrent ) const
    {
        const mt::mat4f & wm = this->getWorldMatrix();

        mt::vec2f minimal( 0.f, 0.f );
        mt::vec2f maximal( m_width, m_height );

        mt::vec2f minimal_wm;
        mt::mul_v2_v2_m4( minimal_wm, minimal, wm );

        mt::vec2f maximal_wm;
        mt::mul_v2_v2_m4( maximal_wm, maximal, wm );

        mt::set_box_from_two_point( _boundingBox, minimal_wm, maximal_wm );

        *_boundingBoxCurrent = &_boundingBox;
    }
}
