#include "AffectorTransformationRotateTime.h"

#include "Kernel/AssertionNotImplemented.h"

#include "math/angle.h"
#include "math/utils.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AffectorTransformationRotateTime::AffectorTransformationRotateTime( GOAP::NodeInterface * _node, const LambdaAffectorTransformationRotateTime & _transformCb, float _from, float _to, float _time, ETransformationRotateMode _mode )
        : m_node( _node )
        , m_transformCb( _transformCb )
        , m_from( _from )
        , m_to( _to )
        , m_progress( 0.f )
        , m_time( _time )
        , m_mode( _mode )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AffectorTransformationRotateTime::~AffectorTransformationRotateTime()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AffectorTransformationRotateTime::_affect( const UpdateContext * _context, float * const _used )
    {
        float time = _context->time;

        if( m_progress + time < m_time )
        {
            *_used = time;

            m_progress += time;

            float t = m_progress / m_time;

            float current = mt::linerp_f1( m_from, m_to, t );

            m_transformCb( current );

            return false;
        }

        *_used = m_time - m_progress;

        m_transformCb( m_to );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AffectorTransformationRotateTime::_prepare()
    {
        switch( m_mode )
        {
        case ETRANSFORMATION_ROTATE_MODE_INTERPOLATE:
            {
                mt::angle_correct_interpolate_from_to( m_from, m_to, &m_from, &m_to );
            } break;
        case ETRANSFORMATION_ROTATE_MODE_CW:
            {
                while( m_to > m_from )
                {
                    m_from += mt::constant::two_pi;
                    m_to -= mt::constant::two_pi;
                }
            } break;
        case ETRANSFORMATION_ROTATE_MODE_CCW:
            {
                while( m_from > m_to )
                {
                    m_to += mt::constant::two_pi;
                    m_from -= mt::constant::two_pi;
                }                
            } break;
        default:
            {
                MENGINE_ASSERTION_NOT_IMPLEMENTED();
            } break;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void AffectorTransformationRotateTime::_complete( bool _isEnd )
    {
        bool skiped = (_isEnd == false);

        m_node->complete( true, skiped );

        m_node = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
}