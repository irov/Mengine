#include "TaskAnimatablePlayReceiver.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    TaskAnimatablePlayReceiver::TaskAnimatablePlayReceiver()
        : m_task( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TaskAnimatablePlayReceiver::~TaskAnimatablePlayReceiver()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void TaskAnimatablePlayReceiver::setTask( GOAP::Task * _task )
    {
        m_task = _task;
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

        m_task->complete();
    }
    //////////////////////////////////////////////////////////////////////////
    void TaskAnimatablePlayReceiver::onAnimationEnd( uint32_t _enumerator )
    {
        MENGINE_UNUSED( _enumerator );

        m_task->complete();
    }
    //////////////////////////////////////////////////////////////////////////
    void TaskAnimatablePlayReceiver::onAnimationInterrupt( uint32_t _enumerator )
    {
        MENGINE_UNUSED( _enumerator );

        m_task->complete();
    }
}
