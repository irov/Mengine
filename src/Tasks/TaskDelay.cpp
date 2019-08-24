#include "TaskDelay.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class TaskDelay::TaskDelayScheduleEvent
        : public SchedulerEventInterface
        , public Mengine::Factorable
    {
    public:
        explicit TaskDelayScheduleEvent( TaskDelay * _task )
            : m_task( _task )
        {
        }

        ~TaskDelayScheduleEvent() override
        {
        }

    protected:
        void onSchedulerComplete( uint32_t _id ) override
        {
            m_task->schedulerComplete( _id );
        }

        void onSchedulerStop( uint32_t _id ) override
        {
            m_task->schedulerComplete( _id );
        }

    protected:
        TaskDelay * m_task;
    };
    //////////////////////////////////////////////////////////////////////////
    TaskDelay::TaskDelay( const SchedulerPtr & _scheduler, float _time )
        : m_scheduler( _scheduler )
        , m_time( _time )
        , m_id( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TaskDelay::~TaskDelay()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void TaskDelay::schedulerComplete( uint32_t _id )
    {
        if( m_id != _id )
        {
            return;
        }

        m_id = 0;

        this->complete();
    }
    //////////////////////////////////////////////////////////////////////////
    bool TaskDelay::_onRun()
    {
        SchedulerEventInterfacePtr ev = Helper::makeFactorableUnique<TaskDelay::TaskDelayScheduleEvent>( this );

        uint32_t id = m_scheduler->event( m_time, ev );

        if( id == 0 )
        {
            LOGGER_ERROR( "error schedule event" );

            return true;
        }

        m_id = id;

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void TaskDelay::_onFinally()
    {
        if( m_id != 0 )
        {
            m_scheduler->remove( m_id );
            m_id = 0;
        }

        m_scheduler = nullptr;
    }
}