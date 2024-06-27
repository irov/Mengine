#include "TaskLocalDelay.h"

#include "Interface/PlayerServiceInterface.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/Logger.h"

#include "GOAP/NodeInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class TaskLocalDelay::ScheduleEvent
        : public SchedulerEventInterface
        , public Mengine::Factorable
    {
    public:
        explicit ScheduleEvent( TaskLocalDelay * _task, GOAP::NodeInterface * _node )
            : m_task( _task )
            , m_node( _node )
        {
        }

        ~ScheduleEvent() override
        {
        }

    protected:
        void onSchedulerComplete( UniqueId _id ) override
        {
            m_task->schedulerComplete( _id );

            m_node->complete();
        }

        void onSchedulerStop( UniqueId _id ) override
        {
            m_task->schedulerComplete( _id );

            m_node->complete();
        }

    protected:
        TaskLocalDelay * m_task;
        GOAP::NodeInterfacePtr m_node;
    };
    //////////////////////////////////////////////////////////////////////////
    TaskLocalDelay::TaskLocalDelay( GOAP::Allocator * _allocator, float _time, const DocumentInterfacePtr & _doc )
        : GOAP::TaskInterface( _allocator )
        , m_time( _time )
#if defined(MENGINE_DOCUMENT_ENABLE)
        , m_doc( _doc )
#endif
        , m_id( INVALID_UNIQUE_ID )
    {
        MENGINE_UNUSED( _doc );
    }
    //////////////////////////////////////////////////////////////////////////
    TaskLocalDelay::~TaskLocalDelay()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void TaskLocalDelay::schedulerComplete( UniqueId _id )
    {
        if( m_id != _id )
        {
            return;
        }

        m_id = INVALID_UNIQUE_ID;
    }
    //////////////////////////////////////////////////////////////////////////
    bool TaskLocalDelay::_onRun( GOAP::NodeInterface * _node )
    {
        SchedulerEventInterfacePtr ev = Helper::makeFactorableUnique<TaskLocalDelay::ScheduleEvent>( MENGINE_DOCUMENT_VALUE( m_doc, nullptr ), this, _node );

        const SchedulerInterfacePtr & scheduler = PLAYER_SERVICE()
            ->getLocalScheduler();

        UniqueId id = scheduler->event( m_time, ev, MENGINE_DOCUMENT_VALUE( m_doc, nullptr ) );

        if( id == INVALID_UNIQUE_ID )
        {
            LOGGER_ERROR( "error schedule event" );

            return true;
        }

        m_id = id;

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    void TaskLocalDelay::_onFinally()
    {
        if( m_id != INVALID_UNIQUE_ID )
        {
            const SchedulerInterfacePtr & scheduler = PLAYER_SERVICE()
                ->getLocalScheduler();

            scheduler->remove( m_id );
            m_id = INVALID_UNIQUE_ID;
        }

#if defined(MENGINE_DOCUMENT_ENABLE)
        m_doc = nullptr;
#endif
    }
    //////////////////////////////////////////////////////////////////////////
}