#pragma once

#include "Interface/SchedulerInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/Updatable.h"
#include "Kernel/BaseUpdation.h"

#include "Config/Vector.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Scheduler
        : public SchedulerInterface
        , public Factorable
        , public Updatable
        , public BaseUpdation
    {
    public:
        DECLARE_UPDATABLE();

    public:
        Scheduler();
        ~Scheduler() override;

    public:
        bool initialize() override;
        void finalize() override;

    public:
        void setName( const ConstString & _name ) override;
        const ConstString & getName() const override;

    public:
        uint32_t event( float _delay, const ScheduleEventInterfacePtr & _event ) override;

    public:
        uint32_t timing( const SchedulePipeInterfacePtr & _pipe, const ScheduleTimingInterfacePtr & _timing, const ScheduleEventInterfacePtr & _event ) override;

    public:
        bool refresh( uint32_t _id ) override;

    public:
        bool exist( uint32_t _id ) const override;

    public:
        bool remove( uint32_t _id ) override;
        void removeAll() override;

        bool freeze( uint32_t _id, bool _freeze ) override;
        void freezeAll( bool _freeze ) override;
        bool isFreezeAll() const override;
        bool isFreeze( uint32_t _id ) const override;

        float getTimePassed( uint32_t _id ) const override;
        float getTimeLeft( uint32_t _id ) const override;

    public:
        void setSpeedFactor( float _factor ) override;
        float getSpeedFactor() const override;

    public:
        float getTime() const override;

    protected:
        void update( const UpdateContext * _context ) override;

    protected:
        ConstString m_name;

        uint32_t m_updataterId;

        float m_speedFactor;
        float m_time;

        enum ESchduleType
        {
            EST_EVENT,
            EST_TIMING
        };

        struct ScheduleEventDesc
        {
            uint32_t id;

            ScheduleEventInterfacePtr event;
            ScheduleTimingInterfacePtr timer;
            SchedulePipeInterfacePtr pipe;

            float delay;
            float time_delay;

            uint32_t iterate;

            ESchduleType type;
            bool dead;
            bool freeze;
            bool iterate_invalide;
        };

        typedef Vector<ScheduleEventDesc> ListSchedules;
        ListSchedules m_schedules;
        ListSchedules m_schedulesAdd;

        bool m_freezeAll;
        bool m_update;

    protected:
        bool removeSchedule_( ScheduleEventDesc & _event );
        bool findScheduleEvent_( uint32_t _id, const ScheduleEventDesc *& _desc ) const;
        bool findScheduleEvent_( uint32_t _id, ScheduleEventDesc *& _desc );
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Scheduler> SchedulerPtr;
    //////////////////////////////////////////////////////////////////////////
}
