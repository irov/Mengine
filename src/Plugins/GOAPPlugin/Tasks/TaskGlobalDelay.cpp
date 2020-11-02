#include "TaskGlobalDelay.h"

#include "Interface/PlayerServiceInterface.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/Logger.h"

#include "GOAP/NodeInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class TaskGlobalDelay::ScheduleEvent
        : public SchedulerEventInterface
        , public Mengine::Factorable
    {
    public:
        explicit ScheduleEvent( TaskGlobalDelay * _task, GOAP::NodeInterface * _node )
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
        TaskGlobalDelay * m_task;
        GOAP::NodeInterfacePtr m_node;
    };
    //////////////////////////////////////////////////////////////////////////
    TaskGlobalDelay::TaskGlobalDelay( float _time, const DocumentPtr & _doc )
        : m_time( _time )
        , m_doc( _doc )
        , m_id( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TaskGlobalDelay::~TaskGlobalDelay()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void TaskGlobalDelay::schedulerComplete( uint32_t _id )
    {
        if( m_id != _id )
        {
            return;
        }

        m_id = 0;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TaskGlobalDelay::_onRun( GOAP::NodeInterface * _node )
    {
        SchedulerEventInterfacePtr ev = Helper::makeFactorableUnique<TaskGlobalDelay::ScheduleEvent>( m_doc, this, _node );

        const SchedulerInterfacePtr & scheduler = PLAYER_SERVICE()
            ->getGlobalScheduler();

        uint32_t id = scheduler->event( m_time, ev, m_doc );

        if( id == 0 )
        {
            LOGGER_ERROR( "error schedule event" );

            return true;
        }

        m_id = id;

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void TaskGlobalDelay::_onFinally()
    {
        if( m_id != 0 )
        {
            const SchedulerInterfacePtr & scheduler = PLAYER_SERVICE()
                ->getGlobalScheduler();

            scheduler->remove( m_id );
            m_id = 0;
        }
    }
}