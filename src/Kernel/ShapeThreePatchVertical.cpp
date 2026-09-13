#include "ShapeThreePatchVertical.h"

#include "Kernel/Surface.h"

#include "math/box2.h"
#include "math/uv4.h"

namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static const RenderIndex SHAPE_THREE_PATCH_VERTICAL_INDICES[18] = {
            0, 1, 3, 0, 3, 2,
            2, 3, 5, 2, 5, 4,
            4, 5, 7, 4, 7, 6,
        };
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    ShapeThreePatchVertical::ShapeThreePatchVertical()
        : m_size( -1.f, -1.f )
        , m_capInsets( 0.f, 0.f )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ShapeThreePatchVertical::~ShapeThreePatchVertical()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void ShapeThreePatchVertical::setSize( const mt::vec2f & _size )
    {
        if( m_size == _size )
        {
            return;
        }

        m_size = _size;

        this->invalidateVerticesLocal();
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & ShapeThreePatchVertical::getSize() const
    {
        return m_size;
    }
    //////////////////////////////////////////////////////////////////////////
    void ShapeThreePatchVertical::removeSize()
    {
        this->setSize( mt::vec2f( -1.f, -1.f ) );
    }
    //////////////////////////////////////////////////////////////////////////
    bool ShapeThreePatchVertical::hasSize() const
    {
        return m_size.x >= 0.f || m_size.y >= 0.f;
    }
    //////////////////////////////////////////////////////////////////////////
    void ShapeThreePatchVertical::setCapInsets( const mt::vec2f & _capInsets )
    {
        mt::vec2f capInsets(
            _capInsets.x < 0.f ? 0.f : _capInsets.x,
            _capInsets.y < 0.f ? 0.f : _capInsets.y
        );

        if( m_capInsets == capInsets )
        {
            return;
        }

        m_capInsets = capInsets;

        this->invalidateVerticesLocal();
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & ShapeThreePatchVertical::getCapInsets() const
    {
        return m_capInsets;
    }
    //////////////////////////////////////////////////////////////////////////
    mt::vec2f ShapeThreePatchVertical::getMinimumSize() const
    {
        return mt::vec2f( 0.f, m_capInsets.x + m_capInsets.y );
    }
    //////////////////////////////////////////////////////////////////////////
    void ShapeThreePatchVertical::updateVerticesLocal() const
    {
        const mt::vec2f & sourceSize = m_surface->getSize();

        float top = m_capInsets.x;
        float bottom = m_capInsets.y;

        float insets = top + bottom;

        if( insets > sourceSize.y && insets > 0.f )
        {
            float scale = sourceSize.y / insets;

            top *= scale;
            bottom *= scale;
        }

        float targetWidth = m_size.x >= 0.f ? m_size.x : sourceSize.x;
        float targetHeight = m_size.y >= 0.f ? m_size.y : sourceSize.y;

        float minimumHeight = top + bottom;

        if( targetHeight < minimumHeight )
        {
            targetHeight = minimumHeight;
        }

        const mt::vec2f & anchor = m_surface->getAnchor();
        mt::vec2f origin = -anchor * mt::vec2f( targetWidth, targetHeight );

        float positionsX[2] = {
            origin.x,
            origin.x + targetWidth,
        };

        float positionsY[4] = {
            origin.y,
            origin.y + top,
            origin.y + targetHeight - bottom,
            origin.y + targetHeight,
        };

        float sourceHeight = sourceSize.y > 0.f ? sourceSize.y : 1.f;

        float sourceX[2] = {
            0.f,
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
            for( uint32_t column = 0; column != 2; ++column )
            {
                uint32_t vertexIndex = column + row * 2;

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
    void ShapeThreePatchVertical::updateVerticesWM() const
    {
        const mt::mat4f & wm = this->getWorldMatrix();

        for( uint32_t index = 0; index != 8; ++index )
        {
            const mt::vec2f & position = m_verticesLocal[index];

            mt::vec3f & wmPosition = m_verticesWM[index].position;
            mt::mul_v3_v2_m4( &wmPosition, position, wm );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void ShapeThreePatchVertical::updateVerticesColor() const
    {
        const Mengine::RenderInterface * render = this->getRender();

        Color color;
        render->calcTotalColor( &color );

        const Color & surfaceColor = m_surface->getColor();

        color *= surfaceColor;

        ColorValue_ARGB argb = color.getAsARGB();

        for( uint32_t index = 0; index != 8; ++index )
        {
            m_verticesWM[index].color = argb;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void ShapeThreePatchVertical::render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const
    {
        const RenderVertex2D * vertices = this->getVerticesWM();

        const RenderMaterialInterfacePtr & material = m_surface->getMaterial();
        const RenderProgramVariableInterfacePtr & programVariable = m_surface->getProgramVariable();

        const Mengine::RenderInterface * render = this->getRender();

        const mt::box2f * bb = render->getBoundingBox();

        _renderPipeline->addRenderObject( _context, material, programVariable, vertices, 8, Detail::SHAPE_THREE_PATCH_VERTICAL_INDICES, 18, bb, EROF_NONE, MENGINE_DOCUMENT_FORWARD );
    }
    //////////////////////////////////////////////////////////////////////////
    void ShapeThreePatchVertical::_updateBoundingBox( mt::box2f * const _boundingBox, mt::box2f ** const _boundingBoxCurrent ) const
    {
        const RenderVertex2D * vertices = this->getVerticesWM();

        mt::box2_reset( _boundingBox, vertices[0].position.x, vertices[0].position.y );

        for( uint32_t index = 1; index != 8; ++index )
        {
            const RenderVertex2D & vertex = vertices[index];

            mt::box2_add_internal_point( _boundingBox, vertex.position.x, vertex.position.y );
        }

        *_boundingBoxCurrent = _boundingBox;
    }
    //////////////////////////////////////////////////////////////////////////
}
