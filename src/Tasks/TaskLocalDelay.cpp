#include "TaskLocalDelay.h"

#include "Interface/PlayerServiceInterface.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class TaskLocalDelay::ScheduleEvent
        : public SchedulerEventInterface
        , public Mengine::Factorable
    {
    public:
        explicit ScheduleEvent( TaskLocalDelay * _task )
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
        TaskLocalDelay * m_task;
    };
    //////////////////////////////////////////////////////////////////////////
    TaskLocalDelay::TaskLocalDelay( float _time )
        : m_time( _time )
        , m_id( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    TaskLocalDelay::~TaskLocalDelay()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void TaskLocalDelay::schedulerComplete( uint32_t _id )
    {
        if( m_id != _id )
        {
            return;
        }

        m_id = 0;

        this->complete();
    }
    //////////////////////////////////////////////////////////////////////////
    bool TaskLocalDelay::_onRun()
    {
        SchedulerEventInterfacePtr ev = Helper::makeFactorableUnique<TaskLocalDelay::ScheduleEvent>( this );

        const SchedulerInterfacePtr & scheduler = PLAYER_SERVICE()
            ->getScheduler();

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
    void TaskLocalDelay::_onFinally()
    {
        if( m_id != 0 )
        {
            const SchedulerInterfacePtr & scheduler = PLAYER_SERVICE()
                ->getScheduler();

            scheduler->remove( m_id );
            m_id = 0;
        }
    }
}