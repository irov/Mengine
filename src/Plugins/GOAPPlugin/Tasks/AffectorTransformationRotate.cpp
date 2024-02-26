#include "AffectorTransformationRotate.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AffectorTransformationRotate::AffectorTransformationRotate( GOAP::NodeInterface * _node, const LambdaAffectorTransformationRotate & _transformCb, float _from, float _to, float _speed )
        : m_node( _node )
        , m_transformCb( _transformCb )
        , m_from( _from )
        , m_to( _to )
        , m_progress( _from )
        , m_speed( _speed )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AffectorTransformationRotate::~AffectorTransformationRotate()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AffectorTransformationRotate::_affect( const UpdateContext * _context, float * const _used )
    {
        float time = _context->time;

        float delta = m_to - m_from;

        float add = delta * m_speed * time;

        if( m_progress + add < m_to )
        {
            m_progress += add;

            *_used = time;

            m_transformCb( m_progress );

            return false;
        }

        *_used = time * (m_to - m_progress) / add;

        m_progress = m_to;

        m_transformCb( m_progress );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AffectorTransformationRotate::_complete( bool _isEnd )
    {
        bool skiped = (_isEnd == false);

        m_node->complete( true, skiped );

        m_node = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
}