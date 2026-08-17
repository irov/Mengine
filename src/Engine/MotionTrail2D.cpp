#include "MotionTrail2D.h"

#include "Interface/RenderPipelineInterface.h"
#include "Interface/TransformationInterface.h"

#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/RenderVertex2D.h"
#include "Kernel/VectorRenderIndex.h"
#include "Kernel/VectorRenderVertex2D.h"

#include "math/box2.h"

#include <cmath>
#include <limits>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MotionTrail2D::MotionTrail2D()
        : m_width( 16.f )
        , m_fadeTime( 0.5f )
        , m_minSegmentLength( 2.f )
        , m_emission( true )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MotionTrail2D::~MotionTrail2D()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void MotionTrail2D::setResourceImage( const ResourceImagePtr & _resourceImage )
    {
        if( m_resourceImage == _resourceImage )
        {
            return;
        }

        this->recompile( [this, &_resourceImage]()
        {
            m_resourceImage = _resourceImage;
            return m_resourceImage != nullptr;
        } );
    }
    //////////////////////////////////////////////////////////////////////////
    const ResourceImagePtr & MotionTrail2D::getResourceImage() const
    {
        return m_resourceImage;
    }
    //////////////////////////////////////////////////////////////////////////
    void MotionTrail2D::setTarget( const TransformablePtr & _target )
    {
        m_target = _target;
    }
    //////////////////////////////////////////////////////////////////////////
    const TransformablePtr & MotionTrail2D::getTarget() const
    {
        return m_target;
    }
    //////////////////////////////////////////////////////////////////////////
    void MotionTrail2D::setWidth( float _width )
    {
        float width = _width < 0.f ? 0.f : _width;

        if( m_width == width )
        {
            return;
        }

        m_width = width;
        this->invalidateBoundingBox();
    }
    //////////////////////////////////////////////////////////////////////////
    float MotionTrail2D::getWidth() const
    {
        return m_width;
    }
    //////////////////////////////////////////////////////////////////////////
    void MotionTrail2D::setFadeTime( float _fadeTime )
    {
        m_fadeTime = _fadeTime < 0.f ? 0.f : _fadeTime;
    }
    //////////////////////////////////////////////////////////////////////////
    float MotionTrail2D::getFadeTime() const
    {
        return m_fadeTime;
    }
    //////////////////////////////////////////////////////////////////////////
    void MotionTrail2D::setMinSegmentLength( float _length )
    {
        m_minSegmentLength = _length < 0.f ? 0.f : _length;
    }
    //////////////////////////////////////////////////////////////////////////
    float MotionTrail2D::getMinSegmentLength() const
    {
        return m_minSegmentLength;
    }
    //////////////////////////////////////////////////////////////////////////
    void MotionTrail2D::setEmission( bool _emission )
    {
        m_emission = _emission;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MotionTrail2D::getEmission() const
    {
        return m_emission;
    }
    //////////////////////////////////////////////////////////////////////////
    void MotionTrail2D::addPoint( const mt::vec2f & _position )
    {
        this->appendPoint_( _position );
    }
    //////////////////////////////////////////////////////////////////////////
    void MotionTrail2D::clearTrail()
    {
        m_points.clear();
        this->invalidateBoundingBox();
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t MotionTrail2D::getPointCount() const
    {
        return (uint32_t)m_points.size();
    }
    //////////////////////////////////////////////////////////////////////////
    bool MotionTrail2D::_compile()
    {
        return m_resourceImage != nullptr && m_resourceImage->compile() == true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MotionTrail2D::_release()
    {
        if( m_resourceImage != nullptr )
        {
            m_resourceImage->release();
        }

        this->releaseMaterial();
    }
    //////////////////////////////////////////////////////////////////////////
    void MotionTrail2D::_dispose()
    {
        m_target = nullptr;
        m_resourceImage = nullptr;
        m_points.clear();
        Node::_dispose();
    }
    //////////////////////////////////////////////////////////////////////////
    void MotionTrail2D::update( const UpdateContext * _context )
    {
        for( TrailPoint & point : m_points )
        {
            point.age += _context->time;
        }

        while( m_points.empty() == false && (m_fadeTime <= 0.f || m_points.front().age >= m_fadeTime) )
        {
            m_points.erase( m_points.begin() );
        }

        if( m_emission == true && m_target != nullptr )
        {
            mt::vec3f worldPosition = m_target->getTransformation()->getWorldPosition();
            mt::mat4f worldMatrixInv;
            mt::inv_m4_m4( &worldMatrixInv, this->getWorldMatrix() );

            mt::vec3f localPosition;
            mt::mul_v3_v3_m4( &localPosition, worldPosition, worldMatrixInv );
            this->appendPoint_( mt::vec2f( localPosition.x, localPosition.y ) );
        }

        this->invalidateBoundingBox();
    }
    //////////////////////////////////////////////////////////////////////////
    void MotionTrail2D::render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) const
    {
        uint32_t pointCount = (uint32_t)m_points.size();

        if( pointCount < 2 || m_width <= 0.f )
        {
            return;
        }

        VectorRenderVertex2D vertices( pointCount * 2 );
        VectorRenderIndex indices( (pointCount - 1) * 6 );
        const mt::mat4f & worldMatrix = this->getWorldMatrix();

        Color totalColor;
        this->calcTotalColor( &totalColor );
        totalColor *= m_resourceImage->getColor();
        const mt::uv4f & uvImage = m_resourceImage->getUVTexture( 0 );

        for( uint32_t index = 0; index != pointCount; ++index )
        {
            const mt::vec2f & previous = m_points[index == 0 ? 0 : index - 1].position;
            const mt::vec2f & next = m_points[index + 1 < pointCount ? index + 1 : index].position;
            float dx = next.x - previous.x;
            float dy = next.y - previous.y;
            float length = std::sqrt( dx * dx + dy * dy );
            mt::vec2f normal( 0.f, m_width * 0.5f );

            if( length > 0.00001f )
            {
                float scale = m_width * 0.5f / length;
                normal = mt::vec2f( -dy * scale, dx * scale );
            }

            float alpha = m_fadeTime > 0.f ? 1.f - mt::clamp( 0.f, m_points[index].age / m_fadeTime, 1.f ) : 0.f;
            Color vertexColor = totalColor;
            vertexColor.setA( vertexColor.getA() * alpha );
            ColorValue_ARGB argb = vertexColor.getAsARGB();
            float v = (float)index / (float)(pointCount - 1);
            mt::vec2f localPositions[2] = {m_points[index].position - normal, m_points[index].position + normal};

            for( uint32_t side = 0; side != 2; ++side )
            {
                RenderVertex2D & vertex = vertices[index * 2 + side];
                mt::vec3f local( localPositions[side].x, localPositions[side].y, 0.f );
                mt::mul_v3_v3_m4( &vertex.position, local, worldMatrix );
                vertex.color = argb;
                const mt::vec2f & uvBegin = uvImage[side];
                const mt::vec2f & uvEnd = uvImage[side == 0 ? 3 : 2];
                vertex.uv[0].x = uvBegin.x + (uvEnd.x - uvBegin.x) * v;
                vertex.uv[0].y = uvBegin.y + (uvEnd.y - uvBegin.y) * v;
                vertex.uv[1] = vertex.uv[0];
            }
        }

        for( uint32_t index = 0; index + 1 < pointCount; ++index )
        {
            uint32_t vo = index * 2;
            uint32_t io = index * 6;
            indices[io + 0] = (RenderIndex)(vo + 0);
            indices[io + 1] = (RenderIndex)(vo + 2);
            indices[io + 2] = (RenderIndex)(vo + 1);
            indices[io + 3] = (RenderIndex)(vo + 1);
            indices[io + 4] = (RenderIndex)(vo + 2);
            indices[io + 5] = (RenderIndex)(vo + 3);
        }

        _renderPipeline->addRenderObject( _context, this->getMaterial(), nullptr, vertices.data(), (uint32_t)vertices.size(), indices.data(), (uint32_t)indices.size(), this->getBoundingBox(), false, MENGINE_DOCUMENT_FORWARD );
    }
    //////////////////////////////////////////////////////////////////////////
    RenderMaterialInterfacePtr MotionTrail2D::_updateMaterial() const
    {
        RenderMaterialInterfacePtr material = this->makeImageMaterial( m_resourceImage, false, MENGINE_DOCUMENT_FACTORABLE );
        MENGINE_ASSERTION_MEMORY_PANIC( material, "motion trail '%s' material is null", this->getName().c_str() );
        return material;
    }
    //////////////////////////////////////////////////////////////////////////
    void MotionTrail2D::_updateBoundingBox( mt::box2f * const _boundingBox, mt::box2f ** const _boundingBoxCurrent ) const
    {
        mt::vec2f minimum( 0.f, 0.f );
        mt::vec2f maximum( 0.f, 0.f );

        if( m_points.empty() == false )
        {
            minimum = m_points.front().position;
            maximum = minimum;

            for( const TrailPoint & point : m_points )
            {
                minimum.x = point.position.x < minimum.x ? point.position.x : minimum.x;
                minimum.y = point.position.y < minimum.y ? point.position.y : minimum.y;
                maximum.x = point.position.x > maximum.x ? point.position.x : maximum.x;
                maximum.y = point.position.y > maximum.y ? point.position.y : maximum.y;
            }

            float halfWidth = m_width * 0.5f;
            minimum -= mt::vec2f( halfWidth, halfWidth );
            maximum += mt::vec2f( halfWidth, halfWidth );
        }

        const mt::vec2f corners[4] = {
            mt::vec2f( minimum.x, minimum.y ),
            mt::vec2f( maximum.x, minimum.y ),
            mt::vec2f( maximum.x, maximum.y ),
            mt::vec2f( minimum.x, maximum.y )
        };

        mt::vec2f worldPoint;
        mt::mul_v2_v2_m4( &worldPoint, corners[0], this->getWorldMatrix() );
        mt::box2_reset( _boundingBox, worldPoint.x, worldPoint.y );

        for( uint32_t index = 1; index != 4; ++index )
        {
            mt::mul_v2_v2_m4( &worldPoint, corners[index], this->getWorldMatrix() );
            mt::box2_add_internal_point( _boundingBox, worldPoint.x, worldPoint.y );
        }

        *_boundingBoxCurrent = _boundingBox;
    }
    //////////////////////////////////////////////////////////////////////////
    void MotionTrail2D::appendPoint_( const mt::vec2f & _position )
    {
        if( m_points.empty() == false )
        {
            const mt::vec2f & last = m_points.back().position;
            float dx = _position.x - last.x;
            float dy = _position.y - last.y;

            if( dx * dx + dy * dy < m_minSegmentLength * m_minSegmentLength )
            {
                return;
            }
        }

        const uint32_t maxPoints = (uint32_t)std::numeric_limits<RenderIndex>::max() / 2U;

        if( m_points.size() >= maxPoints )
        {
            m_points.erase( m_points.begin() );
        }

        m_points.emplace_back( TrailPoint{_position, 0.f} );
        this->invalidateBoundingBox();
    }
    //////////////////////////////////////////////////////////////////////////
}
