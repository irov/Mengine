#include "TaskDelay.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/Logger.h"

#include "GOAP/NodeInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class TaskDelay::ScheduleEvent
        : public SchedulerEventInterface
        , public Mengine::Factorable
    {
    public:
        explicit ScheduleEvent( TaskDelay * _task, GOAP::NodeInterface * _node )
            : m_task( _task )
            , m_node( _node )
        {
        }

        ~ScheduleEvent() override
        {
        }

    protected:
        void onSchedulerComplete( uint32_t _id ) override
        {
            m_task->schedulerComplete( _id );

            m_node->complete();
        }

        void onSchedulerStop( uint32_t _id ) override
        {
            m_task->schedulerComplete( _id );

            m_node->complete();
        }

    protected:
        TaskDelay * m_task;
        GOAP::NodeInterfacePtr m_node;
    };
    //////////////////////////////////////////////////////////////////////////
    TaskDelay::TaskDelay( const SchedulerInterfacePtr & _scheduler, float _time, const DocumentPtr & _doc )
        : m_scheduler( _scheduler )
        , m_time( _time )
        , m_doc( _doc )
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
    }
    //////////////////////////////////////////////////////////////////////////
    bool TaskDelay::_onRun( GOAP::NodeInterface * _node )
    {
        SchedulerEventInterfacePtr ev = Helper::makeFactorableUnique<TaskDelay::ScheduleEvent>( m_doc, this, _node );

        uint32_t id = m_scheduler->event( m_time, ev, m_doc );

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