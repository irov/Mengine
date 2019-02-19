#include "SchedulerHelper.h"

#include "Kernel/FactorableUnique.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        namespace Detail
        {
            //////////////////////////////////////////////////////////////////////////
            class HelperScheduleEvent
                : public ScheduleEventInterface
                , public Factorable
            {
            public:
                HelperScheduleEvent( const LambdaScheduleEvent & _event )
                    : m_event( _event )
                {
                }

                ~HelperScheduleEvent() override
                {
                }

            protected:
                void onScheduleComplete( uint32_t _id ) override
                {
                    m_event( _id, true );
                }

                void onScheduleStop( uint32_t _id ) override
                {
                    m_event( _id, false );
                }

            protected:
                LambdaScheduleEvent m_event;
            };
            //////////////////////////////////////////////////////////////////////////
            class HelperSchedulePipe
                : public SchedulePipeInterface
                , public Factorable
            {
            public:
                HelperSchedulePipe( const LambdaSchedulePipe & _pipe )
                    : m_pipe( _pipe )
                {
                }

                ~HelperSchedulePipe() override
                {
                }

            protected:
                float onSchedulePipe( uint32_t _id, uint32_t _index ) override
                {
                    float delay = m_pipe( _id, _index );

                    return delay;
                }

            protected:
                LambdaSchedulePipe m_pipe;
            };
            //////////////////////////////////////////////////////////////////////////
            class HelperScheduleTiming
                : public ScheduleTimingInterface
                , public Factorable
            {
            public:
                HelperScheduleTiming( const LambdaScheduleTiming & _timing )
                    : m_timing( _timing )
                {
                }

                ~HelperScheduleTiming() override
                {
                }

            protected:
                void onScheduleTiming( uint32_t _id, uint32_t _iterate, float _time ) override
                {
                    m_timing( _id, _iterate, _time );
                }

            protected:
                LambdaScheduleTiming m_timing;
            };
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t schedulerEvent( const SchedulerInterfacePtr & _scheduler, float _delay, const LambdaScheduleEvent & _event )
        {
            ScheduleEventInterfacePtr event = Helper::makeFactorableUnique<Detail::HelperScheduleEvent>( _event );

            uint32_t id = _scheduler->event( _delay, event );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t schedulerTiming( const SchedulerInterfacePtr & _scheduler, const LambdaSchedulePipe & _pipe, const LambdaScheduleTiming & _timing, const LambdaScheduleEvent & _event )
        {
            SchedulePipeInterfacePtr pipe = Helper::makeFactorableUnique<Detail::HelperSchedulePipe>( _pipe );
            ScheduleTimingInterfacePtr timing = Helper::makeFactorableUnique<Detail::HelperScheduleTiming>( _timing );
            ScheduleEventInterfacePtr event = _event != nullptr ? Helper::makeFactorableUnique<Detail::HelperScheduleEvent>( _event ) : nullptr;

            uint32_t id = _scheduler->timing( pipe, timing, event );

            return id;
        }
    }
}