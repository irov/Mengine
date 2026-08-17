#include "AffectorTransformationPath.h"

#include "Interface/TransformationInterface.h"

#include <cmath>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AffectorTransformationPath::AffectorTransformationPath( GOAP::NodeInterface * _node, const TransformablePtr & _transformable, const Vector<mt::vec3f> & _points, float _duration, ETransformationPathMode _mode )
        : m_node( _node )
        , m_transformable( _transformable )
        , m_points( _points )
        , m_duration( _duration )
        , m_elapsed( 0.f )
        , m_mode( _mode )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AffectorTransformationPath::~AffectorTransformationPath()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AffectorTransformationPath::_affect( const UpdateContext * _context, float * const _used )
    {
        if( m_points.empty() == true )
        {
            *_used = 0.f;

            return true;
        }

        if( m_duration <= 0.f )
        {
            *_used = 0.f;

            m_transformable->getTransformation()->setLocalPosition( m_points.back() );

            return true;
        }

        if( this->getFreeze() == true )
        {
            *_used = 0.f;

            return false;
        }

        float speedFactor = this->getSpeedFactor();

        if( speedFactor <= 0.f )
        {
            *_used = 0.f;

            return false;
        }

        float scaledTime = _context->time * speedFactor;
        float remaining = m_duration - m_elapsed;
        bool complete = scaledTime >= remaining;

        if( complete == true )
        {
            m_elapsed = m_duration;
            *_used = remaining / speedFactor;
        }
        else
        {
            m_elapsed += scaledTime;
            *_used = _context->time;
        }

        float t = m_elapsed / m_duration;

        const EasingInterfacePtr & easing = this->getEasing();

        if( easing != nullptr )
        {
            t = easing->easing( t );
        }

        mt::vec3f position;
        this->evaluate_( t, &position );

        m_transformable->getTransformation()->setLocalPosition( position );

        return complete;
    }
    //////////////////////////////////////////////////////////////////////////
    void AffectorTransformationPath::_complete( bool _isEnd )
    {
        bool skipped = (_isEnd == false);

        m_node->complete( true, skipped );

        m_node = nullptr;
        m_transformable = nullptr;
        m_points.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void AffectorTransformationPath::evaluate_( float _t, mt::vec3f * const _position ) const
    {
        float t = mt::clamp( 0.f, _t, 1.f );

        if( m_points.size() == 1 )
        {
            *_position = m_points.front();

            return;
        }

        switch( m_mode )
        {
        case ETPM_BEZIER:
            this->evaluateBezier_( t, _position );
            break;
        case ETPM_CATMULL_ROM:
            this->evaluateCatmullRom_( t, _position );
            break;
        case ETPM_LINEAR:
        default:
            this->evaluateLinear_( t, _position );
            break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void AffectorTransformationPath::evaluateLinear_( float _t, mt::vec3f * const _position ) const
    {
        uint32_t segmentCount = (uint32_t)m_points.size() - 1;
        float scaled = _t * (float)segmentCount;
        uint32_t segment = (uint32_t)scaled;

        if( segment >= segmentCount )
        {
            *_position = m_points.back();

            return;
        }

        mt::linerp_v3( _position, m_points[segment], m_points[segment + 1], scaled - (float)segment );
    }
    //////////////////////////////////////////////////////////////////////////
    void AffectorTransformationPath::evaluateBezier_( float _t, mt::vec3f * const _position ) const
    {
        Vector<mt::vec3f> work = m_points;

        for( uint32_t count = (uint32_t)work.size(); count > 1; --count )
        {
            for( uint32_t index = 0; index + 1 < count; ++index )
            {
                mt::linerp_v3( &work[index], work[index], work[index + 1], _t );
            }
        }

        *_position = work.front();
    }
    //////////////////////////////////////////////////////////////////////////
    void AffectorTransformationPath::evaluateCatmullRom_( float _t, mt::vec3f * const _position ) const
    {
        uint32_t segmentCount = (uint32_t)m_points.size() - 1;
        float scaled = _t * (float)segmentCount;
        uint32_t segment = (uint32_t)scaled;

        if( segment >= segmentCount )
        {
            *_position = m_points.back();

            return;
        }

        const mt::vec3f & p0 = m_points[segment == 0 ? 0 : segment - 1];
        const mt::vec3f & p1 = m_points[segment];
        const mt::vec3f & p2 = m_points[segment + 1];
        const mt::vec3f & p3 = m_points[segment + 2 < m_points.size() ? segment + 2 : segment + 1];
        float t = scaled - (float)segment;
        float t2 = t * t;
        float t3 = t2 * t;

        _position->x = 0.5f * ((2.f * p1.x) + (-p0.x + p2.x) * t + (2.f * p0.x - 5.f * p1.x + 4.f * p2.x - p3.x) * t2 + (-p0.x + 3.f * p1.x - 3.f * p2.x + p3.x) * t3);
        _position->y = 0.5f * ((2.f * p1.y) + (-p0.y + p2.y) * t + (2.f * p0.y - 5.f * p1.y + 4.f * p2.y - p3.y) * t2 + (-p0.y + 3.f * p1.y - 3.f * p2.y + p3.y) * t3);
        _position->z = 0.5f * ((2.f * p1.z) + (-p0.z + p2.z) * t + (2.f * p0.z - 5.f * p1.z + 4.f * p2.z - p3.z) * t2 + (-p0.z + 3.f * p1.z - 3.f * p2.z + p3.z) * t3);
    }
    //////////////////////////////////////////////////////////////////////////
}
