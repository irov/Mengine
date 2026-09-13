#include "SpriteSlice3Vertical.h"

#include "Kernel/Logger.h"

#include "math/box2.h"
#include "math/uv4.h"

namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static const mt::vec2f SPRITE_SLICE3_VERTICAL_ZERO_OFFSET( 0.f, 0.f );
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    SpriteSlice3Vertical::SpriteSlice3Vertical()
        : m_size( -1.f, -1.f )
        , m_invalidateVerticesLocal( true )
        , m_invalidateVerticesWM( true )
        , m_invalidateVerticesColor( true )
    {
        for( uint32_t index = 0; index != 3; ++index )
        {
            m_surfaceRevisions[index] = 0;
            m_partOffsets[index] = mt::vec2f( 0.f, 0.f );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    SpriteSlice3Vertical::~SpriteSlice3Vertical()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void SpriteSlice3Vertical::setSurfacePart( uint32_t _index, const SurfacePtr & _surface )
    {
        if( _index >= 3 )
        {
            LOGGER_ERROR( "shape '%s' invalid part index '%u' [max '3']"
                , this->getName().c_str()
                , _index
            );

            return;
        }

        if( m_surfaces[_index] == _surface )
        {
            return;
        }

        bool surfaceActivated = this->isActivate();

        if( surfaceActivated == true && m_surfaces[_index] != nullptr )
        {
            m_surfaces[_index]->deactivate();
        }

        this->recompile( [this, _index, &_surface]()
        {
            m_surfaces[_index] = _surface;
            m_surfaceRevisions[_index] = 0;

            for( uint32_t index = 0; index != 3; ++index )
            {
                if( m_surfaces[index] != nullptr )
                {
                    return true;
                }
            }

            return false;
        } );

        if( surfaceActivated == true && this->isActivate() == true && m_surfaces[_index] != nullptr )
        {
            m_surfaces[_index]->activate();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    const SurfacePtr & SpriteSlice3Vertical::getSurfacePart( uint32_t _index ) const
    {
        if( _index >= 3 )
        {
            return SurfacePtr::none();
        }

        return m_surfaces[_index];
    }
    //////////////////////////////////////////////////////////////////////////
    void SpriteSlice3Vertical::removeSurfacePart( uint32_t _index )
    {
        this->setSurfacePart( _index, nullptr );
    }
    //////////////////////////////////////////////////////////////////////////
    void SpriteSlice3Vertical::setPartOffset( uint32_t _index, const mt::vec2f & _offset )
    {
        if( _index >= 3 )
        {
            LOGGER_ERROR( "shape '%s' invalid part index '%u' [max '3']"
                , this->getName().c_str()
                , _index
            );

            return;
        }

        if( m_partOffsets[_index] == _offset )
        {
            return;
        }

        m_partOffsets[_index] = _offset;

        this->invalidateVerticesLocal();
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & SpriteSlice3Vertical::getPartOffset( uint32_t _index ) const
    {
        if( _index >= 3 )
        {
            return Detail::SPRITE_SLICE3_VERTICAL_ZERO_OFFSET;
        }

        return m_partOffsets[_index];
    }
    //////////////////////////////////////////////////////////////////////////
    void SpriteSlice3Vertical::setSize( const mt::vec2f & _size )
    {
        if( m_size == _size )
        {
            return;
        }

        m_size = _size;

        this->invalidateVerticesLocal();
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & SpriteSlice3Vertical::getSize() const
    {
        return m_size;
    }
    //////////////////////////////////////////////////////////////////////////
    void SpriteSlice3Vertical::removeSize()
    {
        this->setSize( mt::vec2f( -1.f, -1.f ) );
    }
    //////////////////////////////////////////////////////////////////////////
    bool SpriteSlice3Vertical::hasSize() const
    {
        return m_size.x >= 0.f || m_size.y >= 0.f;
    }
    //////////////////////////////////////////////////////////////////////////
    void SpriteSlice3Vertical::calcBandSizes( float * const _width, float * const _heights ) const
    {
        *_width = 0.f;

        _heights[0] = 0.f;
        _heights[1] = 0.f;
        _heights[2] = 0.f;

        for( uint32_t index = 0; index != 3; ++index )
        {
            const SurfacePtr & surface = m_surfaces[index];

            if( surface == nullptr )
            {
                continue;
            }

            const mt::vec2f & size = surface->getSize();

            if( *_width < size.x )
            {
                *_width = size.x;
            }

            _heights[index] = size.y;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    mt::vec2f SpriteSlice3Vertical::getMinimumSize() const
    {
        float width;
        float heights[3];

        this->calcBandSizes( &width, heights );

        return mt::vec2f( 0.f, heights[0] + heights[2] );
    }
    //////////////////////////////////////////////////////////////////////////
    mt::vec2f SpriteSlice3Vertical::getNaturalSize() const
    {
        float width;
        float heights[3];

        this->calcBandSizes( &width, heights );

        return mt::vec2f( width, heights[0] + heights[1] + heights[2] );
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::box2f * SpriteSlice3Vertical::getBoundingBox() const
    {
        this->synchronizeSurfaceRevision();

        return BoundingBox::getBoundingBox();
    }
    //////////////////////////////////////////////////////////////////////////
    bool SpriteSlice3Vertical::_compile()
    {
        bool hasSurface = false;

        for( uint32_t index = 0; index != 3; ++index )
        {
            const SurfacePtr & surface = m_surfaces[index];

            if( surface == nullptr )
            {
                continue;
            }

            if( surface->compile() == false )
            {
                LOGGER_ERROR( "shape '%s' can`t compile surface '%s' part '%u'"
                    , this->getName().c_str()
                    , surface->getName().c_str()
                    , index
                );

                return false;
            }

            m_surfaceRevisions[index] = surface->getRevision();

            hasSurface = true;
        }

        if( hasSurface == false )
        {
            LOGGER_ERROR( "shape '%s' can`t setup any surface part"
                , this->getName().c_str()
            );

            return false;
        }

        this->invalidateVerticesLocal();
        this->invalidateVerticesColor();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SpriteSlice3Vertical::_release()
    {
        for( uint32_t index = 0; index != 3; ++index )
        {
            if( m_surfaces[index] == nullptr )
            {
                continue;
            }

            m_surfaces[index]->release();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void SpriteSlice3Vertical::_dispose()
    {
        for( uint32_t index = 0; index != 3; ++index )
        {
            m_surfaces[index] = nullptr;
        }

        Node::_dispose();
    }
    //////////////////////////////////////////////////////////////////////////
    bool SpriteSlice3Vertical::_activate()
    {
        for( uint32_t index = 0; index != 3; ++index )
        {
            if( m_surfaces[index] == nullptr )
            {
                continue;
            }

            m_surfaces[index]->activate();
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SpriteSlice3Vertical::_deactivate()
    {
        for( uint32_t index = 0; index != 3; ++index )
        {
            if( m_surfaces[index] == nullptr )
            {
                continue;
            }

            m_surfaces[index]->deactivate();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void SpriteSlice3Vertical::synchronizeSurfaceRevision() const
    {
        for( uint32_t index = 0; index != 3; ++index )
        {
            const SurfacePtr & surface = m_surfaces[index];

            if( surface == nullptr )
            {
                continue;
            }

            uint32_t revision = surface->getRevision();

            if( m_surfaceRevisions[index] == revision )
            {
                continue;
            }

            m_surfaceRevisions[index] = revision;

            this->invalidateVerticesLocal();
            this->invalidateVerticesColor();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void SpriteSlice3Vertical::invalidateVerticesLocal() const
    {
        m_invalidateVerticesLocal = true;

        this->invalidateVerticesWM();
    }
    //////////////////////////////////////////////////////////////////////////
    void SpriteSlice3Vertical::invalidateVerticesWM() const
    {
        m_invalidateVerticesWM = true;

        this->invalidateBoundingBox();
    }
    //////////////////////////////////////////////////////////////////////////
    void SpriteSlice3Vertical::invalidateVerticesColor() const
    {
        m_invalidateVerticesColor = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void SpriteSlice3Vertical::_invalidateColor() const
    {
        this->invalidateVerticesColor();
    }
    //////////////////////////////////////////////////////////////////////////
    void SpriteSlice3Vertical::_invalidateWorldMatrix() const
    {
        this->invalidateVerticesWM();
    }
    //////////////////////////////////////////////////////////////////////////
    void SpriteSlice3Vertical::prepareVerticesWM() const
    {
        this->synchronizeSurfaceRevision();

        if( m_invalidateVerticesLocal == true )
        {
            m_invalidateVerticesLocal = false;

            this->updateVerticesLocal();
        }

        if( m_invalidateVerticesWM == true )
        {
            m_invalidateVerticesWM = false;

            this->updateVerticesWM();
        }

        if( m_invalidateVerticesColor == true )
        {
            m_invalidateVerticesColor = false;

            this->updateVerticesColor();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void SpriteSlice3Vertical::updateVerticesLocal() const
    {
        float bandWidth;
        float bandHeights[3];

        this->calcBandSizes( &bandWidth, bandHeights );

        float minimumHeight = bandHeights[0] + bandHeights[2];

        float targetHeight = m_size.y >= 0.f ? m_size.y : bandHeights[0] + bandHeights[1] + bandHeights[2];

        if( targetHeight < minimumHeight )
        {
            targetHeight = minimumHeight;
        }

        float heights[3] = {bandHeights[0], targetHeight - minimumHeight, bandHeights[2]};

        bool freeWidth = m_size.x < 0.f;

        float targetWidth = freeWidth == true ? bandWidth : m_size.x;

        mt::vec2f anchor( 0.f, 0.f );

        for( uint32_t index = 0; index != 3; ++index )
        {
            if( m_surfaces[index] == nullptr )
            {
                continue;
            }

            anchor = m_surfaces[index]->getAnchor();

            break;
        }

        mt::vec2f origin = -anchor * mt::vec2f( targetWidth, targetHeight );

        float y = origin.y;

        for( uint32_t index = 0; index != 3; ++index )
        {
            const SurfacePtr & surface = m_surfaces[index];

            mt::vec2f painted = surface != nullptr ? surface->getSize() : mt::vec2f( 0.f, 0.f );

            float width = freeWidth == true ? painted.x : targetWidth;
            float height = heights[index];

            const mt::vec2f & offset = m_partOffsets[index];

            float left = origin.x + offset.x;
            float top = y + offset.y;

            uint32_t vertexIndex = index * 4;

            m_verticesLocal[vertexIndex + 0].x = left;
            m_verticesLocal[vertexIndex + 0].y = top;

            m_verticesLocal[vertexIndex + 1].x = left + width;
            m_verticesLocal[vertexIndex + 1].y = top;

            m_verticesLocal[vertexIndex + 2].x = left + width;
            m_verticesLocal[vertexIndex + 2].y = top + height;

            m_verticesLocal[vertexIndex + 3].x = left;
            m_verticesLocal[vertexIndex + 3].y = top + height;

            if( surface != nullptr )
            {
                uint32_t uvCount = surface->getUVCount();

                for( uint32_t uvIndex = 0; uvIndex != uvCount; ++uvIndex )
                {
                    const mt::uv4f & uv = surface->getUV( uvIndex );

                    for( uint32_t v = 0; v != 4; ++v )
                    {
                        m_verticesWM[vertexIndex + v].uv[uvIndex] = uv[v];
                    }
                }
            }

            y += heights[index];
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void SpriteSlice3Vertical::updateVerticesWM() const
    {
        const mt::mat4f & wm = this->getWorldMatrix();

        for( uint32_t index = 0; index != 12; ++index )
        {
            const mt::vec2f & position = m_verticesLocal[index];

            mt::vec3f & wmPosition = m_verticesWM[index].position;
            mt::mul_v3_v2_m4( &wmPosition, position, wm );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void SpriteSlice3Vertical::updateVerticesColor() const
    {
        const Mengine::RenderInterface * render = this->getRender();

        Color totalColor;
        render->calcTotalColor( &totalColor );

        for( uint32_t index = 0; index != 3; ++index )
        {
            const SurfacePtr & surface = m_surfaces[index];

            if( surface == nullptr )
            {
                continue;
            }

            Color color = totalColor;

            const Color & surfaceColor = surface->getColor();

            color *= surfaceColor;

            ColorValue_ARGB argb = color.getAsARGB();

            uint32_t vertexIndex = index * 4;

            for( uint32_t v = 0; v != 4; ++v )
            {
                m_verticesWM[vertexIndex + v].color = argb;
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void SpriteSlice3Vertical::render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const
    {
        const RenderVertex2D * vertices = this->getVerticesWM();

        const Mengine::RenderInterface * render = this->getRender();

        const mt::box2f * bb = render->getBoundingBox();

        for( uint32_t index = 0; index != 3; ++index )
        {
            const SurfacePtr & surface = m_surfaces[index];

            if( surface == nullptr )
            {
                continue;
            }

            const RenderMaterialInterfacePtr & material = surface->getMaterial();
            const RenderProgramVariableInterfacePtr & programVariable = surface->getProgramVariable();

            _renderPipeline->addRenderQuad( _context, material, programVariable, vertices + index * 4, 4u, bb, EROF_NONE, MENGINE_DOCUMENT_FORWARD );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void SpriteSlice3Vertical::_updateBoundingBox( mt::box2f * const _boundingBox, mt::box2f ** const _boundingBoxCurrent ) const
    {
        const RenderVertex2D * vertices = this->getVerticesWM();

        mt::box2_reset( _boundingBox, vertices[0].position.x, vertices[0].position.y );

        for( uint32_t index = 1; index != 12; ++index )
        {
            const RenderVertex2D & vertex = vertices[index];

            mt::box2_add_internal_point( _boundingBox, vertex.position.x, vertex.position.y );
        }

        *_boundingBoxCurrent = _boundingBox;
    }
    //////////////////////////////////////////////////////////////////////////
}
