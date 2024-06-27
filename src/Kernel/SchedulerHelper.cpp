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
                : public SchedulerEventInterface
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
                void onSchedulerComplete( UniqueId _id ) override
                {
                    m_event( _id, true );
                }

                void onSchedulerStop( UniqueId _id ) override
                {
                    m_event( _id, false );
                }

            protected:
                LambdaScheduleEvent m_event;
            };
            //////////////////////////////////////////////////////////////////////////
            class HelperSchedulePipe
                : public SchedulerPipeInterface
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
                float onSchedulerPipe( UniqueId _id, uint32_t _index ) override
                {
                    float delay = m_pipe( _id, _index );

                    return delay;
                }

            protected:
                LambdaSchedulePipe m_pipe;
            };
            //////////////////////////////////////////////////////////////////////////
            class HelperScheduleTiming
                : public SchedulerTimingInterface
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
                void onSchedulerTiming( UniqueId _id, uint32_t _iterate, float _time ) override
                {
                    m_timing( _id, _iterate, _time );
                }

            protected:
                LambdaScheduleTiming m_timing;
            };
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t schedulerEvent( const SchedulerInterfacePtr & _scheduler, float _delay, const LambdaScheduleEvent & _event, const DocumentInterfacePtr & _doc )
        {
            SchedulerEventInterfacePtr event = Helper::makeFactorableUnique<Detail::HelperScheduleEvent>( _doc, _event );

            uint32_t id = _scheduler->event( _delay, event, _doc );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        uint32_t schedulerTiming( const SchedulerInterfacePtr & _scheduler, const LambdaSchedulePipe & _pipe, const LambdaScheduleTiming & _timing, const LambdaScheduleEvent & _event, const DocumentInterfacePtr & _doc )
        {
            SchedulerPipeInterfacePtr pipe = Helper::makeFactorableUnique<Detail::HelperSchedulePipe>( _doc, _pipe );
            SchedulerTimingInterfacePtr timing = Helper::makeFactorableUnique<Detail::HelperScheduleTiming>( _doc, _timing );
            SchedulerEventInterfacePtr event = _event != nullptr ? Helper::makeFactorableUnique<Detail::HelperScheduleEvent>( _doc, _event ) : nullptr;

            uint32_t id = _scheduler->timing( pipe, timing, event, _doc );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}