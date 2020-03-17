#include "TaskAnimatablePlayReceiver.h"

#include "GOAP/NodeInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TaskAnimatablePlayReceiver::TaskAnimatablePlayReceiver()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TaskAnimatablePlayReceiver::~TaskAnimatablePlayReceiver()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void TaskAnimatablePlayReceiver::setGOAPNode( GOAP::NodeInterface * _node )
    {
        m_node = GOAP::NodeInterfacePtr::from( _node );
    }
    //////////////////////////////////////////////////////////////////////////
    void TaskAnimatablePlayReceiver::onAnimationPlay( uint32_t _enumerator, float _time )
    {
        MENGINE_UNUSED( _enumerator );
        MENGINE_UNUSED( _time );
    }
    //////////////////////////////////////////////////////////////////////////
    void TaskAnimatablePlayReceiver::onAnimationRestart( uint32_t _enumerator, float _time )
    {
        MENGINE_UNUSED( _enumerator );
        MENGINE_UNUSED( _time );
    }
    //////////////////////////////////////////////////////////////////////////
    void TaskAnimatablePlayReceiver::onAnimationPause( uint32_t _enumerator )
    {
        MENGINE_UNUSED( _enumerator );
    }
    //////////////////////////////////////////////////////////////////////////
    void TaskAnimatablePlayReceiver::onAnimationResume( uint32_t _enumerator, float _time )
    {
        MENGINE_UNUSED( _enumerator );
        MENGINE_UNUSED( _time );
    }
    //////////////////////////////////////////////////////////////////////////
    void TaskAnimatablePlayReceiver::onAnimationStop( uint32_t _enumerator )
    {
        MENGINE_UNUSED( _enumerator );

        m_node->complete();
    }
    //////////////////////////////////////////////////////////////////////////
    void TaskAnimatablePlayReceiver::onAnimationEnd( uint32_t _enumerator )
    {
        MENGINE_UNUSED( _enumerator );

        m_node->complete();
    }
    //////////////////////////////////////////////////////////////////////////
    void TaskAnimatablePlayReceiver::onAnimationInterrupt( uint32_t _enumerator )
    {
        MENGINE_UNUSED( _enumerator );

        m_node->complete();
    }
}
