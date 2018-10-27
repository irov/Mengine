#pragma once

#include "Interface/ScheduleManagerInterface.h"

#include "Kernel/ServantBase.h"
#include "Kernel/Updatable.h"
#include "Kernel/BaseUpdation.h"

#include "Config/Vector.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum ESchduleType
    {
        EST_EVENT,
        EST_TIMER
    };
    //////////////////////////////////////////////////////////////////////////
    struct ScheduleEventDesc
    {
        uint32_t id;

        ScheduleEventInterfacePtr event;
        ScheduleTimerInterfacePtr timer;
        LambdaPipe pipe;

        float delay;
        float time_delay;

        uint32_t iterate;

        ESchduleType type;
        bool dead;
        bool freeze;
        bool iterate_invalide;
    };
    //////////////////////////////////////////////////////////////////////////
    class ScheduleManager
        : public ServantBase<ScheduleManagerInterface>
        , public Updatable
        , public BaseUpdation
    {
    public:
        DECLARE_UPDATABLE();

    public:
        ScheduleManager();
        ~ScheduleManager() override;

    public:
        bool initialize( const ConstString & _name ) override;
        void finalize() override;

    public:
        const ConstString & getName() const override;

    public:
        uint32_t event( float _delay, const ScheduleEventInterfacePtr & _event ) override;

    public:
        uint32_t timing( const SchedulePipeInterfacePtr & _pipe, const ScheduleTimerInterfacePtr & _timer ) override;

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
        void _update( const UpdateContext * _context ) override;

    protected:
        bool removeSchedule_( ScheduleEventDesc & _event );

    protected:
        ConstString m_name;

        uint32_t m_updataterId;

        float m_speedFactor;
        float m_time;

        typedef Vector<ScheduleEventDesc> ListSchedules;
        ListSchedules m_schedules;
        ListSchedules m_schedulesAdd;

        uint32_t m_enumerator;

        bool m_freezeAll;
        bool m_update;

    protected:
        bool findScheduleEvent_( uint32_t _id, const ScheduleEventDesc *& _desc ) const;
        bool findScheduleEvent_( uint32_t _id, ScheduleEventDesc *& _desc );
    };
}
