#include "ShapeNinePatch.h"

#include "Kernel/Surface.h"

#include "math/box2.h"
#include "math/uv4.h"

namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static const RenderIndex SHAPE_NINE_PATCH_INDICES[54] = {
            0, 1, 5, 0, 5, 4,
            1, 2, 6, 1, 6, 5,
            2, 3, 7, 2, 7, 6,
            4, 5, 9, 4, 9, 8,
            5, 6, 10, 5, 10, 9,
            6, 7, 11, 6, 11, 10,
            8, 9, 13, 8, 13, 12,
            9, 10, 14, 9, 14, 13,
            10, 11, 15, 10, 15, 14,
        };
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    ShapeNinePatch::ShapeNinePatch()
        : m_size( -1.f, -1.f )
        , m_capInsets( 0.f, 0.f, 0.f, 0.f )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ShapeNinePatch::~ShapeNinePatch()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void ShapeNinePatch::setSize( const mt::vec2f & _size )
    {
        if( m_size == _size )
        {
            return;
        }

        m_size = _size;

        this->invalidateVerticesLocal();
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & ShapeNinePatch::getSize() const
    {
        return m_size;
    }
    //////////////////////////////////////////////////////////////////////////
    void ShapeNinePatch::removeSize()
    {
        this->setSize( mt::vec2f( -1.f, -1.f ) );
    }
    //////////////////////////////////////////////////////////////////////////
    bool ShapeNinePatch::hasSize() const
    {
        return m_size.x >= 0.f || m_size.y >= 0.f;
    }
    //////////////////////////////////////////////////////////////////////////
    void ShapeNinePatch::setCapInsets( const mt::vec4f & _capInsets )
    {
        mt::vec4f capInsets(
            _capInsets.x < 0.f ? 0.f : _capInsets.x,
            _capInsets.y < 0.f ? 0.f : _capInsets.y,
            _capInsets.z < 0.f ? 0.f : _capInsets.z,
            _capInsets.w < 0.f ? 0.f : _capInsets.w
        );

        if( m_capInsets == capInsets )
        {
            return;
        }

        m_capInsets = capInsets;

        this->invalidateVerticesLocal();
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec4f & ShapeNinePatch::getCapInsets() const
    {
        return m_capInsets;
    }
    //////////////////////////////////////////////////////////////////////////
    mt::vec2f ShapeNinePatch::getMinimumSize() const
    {
        return mt::vec2f( m_capInsets.x + m_capInsets.z, m_capInsets.y + m_capInsets.w );
    }
    //////////////////////////////////////////////////////////////////////////
    void ShapeNinePatch::updateVerticesLocal() const
    {
        const mt::vec2f & sourceSize = m_surface->getSize();

        float left = m_capInsets.x;
        float top = m_capInsets.y;
        float right = m_capInsets.z;
        float bottom = m_capInsets.w;

        float horizontalInsets = left + right;

        if( horizontalInsets > sourceSize.x && horizontalInsets > 0.f )
        {
            float scale = sourceSize.x / horizontalInsets;

            left *= scale;
            right *= scale;
        }

        float verticalInsets = top + bottom;

        if( verticalInsets > sourceSize.y && verticalInsets > 0.f )
        {
            float scale = sourceSize.y / verticalInsets;

            top *= scale;
            bottom *= scale;
        }

        float targetWidth = m_size.x >= 0.f ? m_size.x : sourceSize.x;
        float targetHeight = m_size.y >= 0.f ? m_size.y : sourceSize.y;

        float minimumWidth = left + right;
        float minimumHeight = top + bottom;

        if( targetWidth < minimumWidth )
        {
            targetWidth = minimumWidth;
        }

        if( targetHeight < minimumHeight )
        {
            targetHeight = minimumHeight;
        }

        const mt::vec2f & anchor = m_surface->getAnchor();
        mt::vec2f origin = -anchor * mt::vec2f( targetWidth, targetHeight );

        float positionsX[4] = {
            origin.x,
            origin.x + left,
            origin.x + targetWidth - right,
            origin.x + targetWidth,
        };

        float positionsY[4] = {
            origin.y,
            origin.y + top,
            origin.y + targetHeight - bottom,
            origin.y + targetHeight,
        };

        float sourceWidth = sourceSize.x > 0.f ? sourceSize.x : 1.f;
        float sourceHeight = sourceSize.y > 0.f ? sourceSize.y : 1.f;

        float sourceX[4] = {
            0.f,
            left / sourceWidth,
            1.f - right / sourceWidth,
            1.f,
        };

        float sourceY[4] = {
            0.f,
            top / sourceHeight,
            1.f - bottom / sourceHeight,
            1.f,
        };

        uint32_t uvCount = m_surface->getUVCount();

        for( uint32_t row = 0; row != 4; ++row )
        {
            for( uint32_t column = 0; column != 4; ++column )
            {
                uint32_t vertexIndex = column + row * 4;

                m_verticesLocal[vertexIndex].x = positionsX[column];
                m_verticesLocal[vertexIndex].y = positionsY[row];

                mt::vec2f uvPoint( sourceX[column], sourceY[row] );

                for( uint32_t uvIndex = 0; uvIndex != uvCount; ++uvIndex )
                {
                    const mt::uv4f & uv = m_surface->getUV( uvIndex );

                    mt::uv4_quad_point( m_verticesWM[vertexIndex].uv + uvIndex, uv, uvPoint );
                }
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void ShapeNinePatch::updateVerticesWM() const
    {
        const mt::mat4f & wm = this->getWorldMatrix();

        for( uint32_t index = 0; index != 16; ++index )
        {
            const mt::vec2f & position = m_verticesLocal[index];

            mt::vec3f & wmPosition = m_verticesWM[index].position;
            mt::mul_v3_v2_m4( &wmPosition, position, wm );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void ShapeNinePatch::updateVerticesColor() const
    {
        const Mengine::RenderInterface * render = this->getRender();

        Color color;
        render->calcTotalColor( &color );

        const Color & surfaceColor = m_surface->getColor();

        color *= surfaceColor;

        ColorValue_ARGB argb = color.getAsARGB();

        for( uint32_t index = 0; index != 16; ++index )
        {
            m_verticesWM[index].color = argb;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void ShapeNinePatch::render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const
    {
        const RenderVertex2D * vertices = this->getVerticesWM();

        const RenderMaterialInterfacePtr & material = m_surface->getMaterial();

        const Mengine::RenderInterface * render = this->getRender();

        const mt::box2f * bb = render->getBoundingBox();

        _renderPipeline->addRenderObject( _context, material, nullptr, vertices, 16, Detail::SHAPE_NINE_PATCH_INDICES, 54, bb, false, MENGINE_DOCUMENT_FORWARD );
    }
    //////////////////////////////////////////////////////////////////////////
    void ShapeNinePatch::_updateBoundingBox( mt::box2f * const _boundingBox, mt::box2f ** const _boundingBoxCurrent ) const
    {
        const RenderVertex2D * vertices = this->getVerticesWM();

        mt::box2_reset( _boundingBox, vertices[0].position.x, vertices[0].position.y );

        for( uint32_t index = 1; index != 16; ++index )
        {
            const RenderVertex2D & vertex = vertices[index];

            mt::box2_add_internal_point( _boundingBox, vertex.position.x, vertex.position.y );
        }

        *_boundingBoxCurrent = _boundingBox;
    }
    //////////////////////////////////////////////////////////////////////////
}
