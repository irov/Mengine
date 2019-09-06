#include "TaskGlobalDelay.h"

#include "Interface/PlayerServiceInterface.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class TaskGlobalDelay::ScheduleEvent
        : public SchedulerEventInterface
        , public Mengine::Factorable
    {
    public:
        explicit ScheduleEvent( TaskGlobalDelay * _task )
            : m_task( _task )
        {
        }

        ~ScheduleEvent() override
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
        TaskGlobalDelay * m_task;
    };
    //////////////////////////////////////////////////////////////////////////
    TaskGlobalDelay::TaskGlobalDelay( float _time )
        : m_time( _time )
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

        this->complete();
    }
    //////////////////////////////////////////////////////////////////////////
    bool TaskGlobalDelay::_onRun()
    {
        SchedulerEventInterfacePtr ev = Helper::makeFactorableUnique<TaskGlobalDelay::ScheduleEvent>( this );

        const SchedulerInterfacePtr & scheduler = PLAYER_SERVICE()
            ->getGlobalScheduler();

        uint32_t id = scheduler->event( m_time, ev );

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