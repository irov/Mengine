#pragma once

#include "Interface/SchedulerInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/Updatable.h"
#include "Kernel/BaseUpdation.h"
#include "Kernel/Vector.h"

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
        uint32_t event( float _delay, const SchedulerEventInterfacePtr & _event, const DocumentPtr & _doc ) override;

    public:
        uint32_t timing( const SchedulerPipeInterfacePtr & _pipe, const SchedulerTimingInterfacePtr & _timing, const SchedulerEventInterfacePtr & _event, const DocumentPtr & _doc ) override;

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

        enum ESchdulerType
        {
            EST_EVENT,
            EST_TIMING
        };

        struct SchedulerEventDesc
        {
            uint32_t id;

            SchedulerEventInterfacePtr event;
            SchedulerTimingInterfacePtr timer;
            SchedulerPipeInterfacePtr pipe;

            float delay;
            float time_delay;

            uint32_t iterate;

            ESchdulerType type;
            bool dead;
            bool freeze;
            bool iterate_invalide;

            DocumentPtr doc;
        };

        typedef Vector<SchedulerEventDesc> VectorSchedulers;
        VectorSchedulers m_schedulersAdd;
        VectorSchedulers m_schedulers;

        bool m_freezeAll;
        bool m_update;

    protected:
        bool removeScheduler_( SchedulerEventDesc * const _event );
        bool findSchedulerEvent_( uint32_t _id, const SchedulerEventDesc ** const _desc ) const;
        bool findSchedulerEvent_( uint32_t _id, SchedulerEventDesc ** const _desc );
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Scheduler> SchedulerPtr;
    //////////////////////////////////////////////////////////////////////////
}
