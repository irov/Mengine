#pragma once

#include "Interface/SchedulerInterface.h"

#include "Kernel/Updatable.h"
#include "Kernel/BaseUpdation.h"
#include "Kernel/Vector.h"

#include "Config/UniqueId.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Scheduler
        : public SchedulerInterface
        , public Updatable
        , protected BaseUpdation
    {
        DECLARE_FACTORABLE( Scheduler );
        DECLARE_UPDATABLE();

    public:
        Scheduler();
        ~Scheduler() override;

    public:
        bool initialize() override;
        void finalize() override;

    public:
        UniqueId event( float _delay, const SchedulerEventInterfacePtr & _event, const DocumentInterfacePtr & _doc ) override;

    public:
        UniqueId timing( const SchedulerPipeInterfacePtr & _pipe, const SchedulerTimingInterfacePtr & _timing, const SchedulerEventInterfacePtr & _event, const DocumentInterfacePtr & _doc ) override;

    public:
        bool refresh( UniqueId _id ) override;

    public:
        bool exist( UniqueId _id ) const override;

    public:
        bool remove( UniqueId _id ) override;
        void removeAll() override;

        bool cancel( UniqueId _id ) override;
        void cancelAll() override;

        bool freeze( UniqueId _id, bool _freeze ) override;
        void freezeAll( bool _freeze ) override;
        bool isFreezeAll() const override;
        bool isFreeze( UniqueId _id ) const override;

        float getTimePassed( UniqueId _id ) const override;
        float getTimeLeft( UniqueId _id ) const override;

    public:
        void setSpeedFactor( float _factor ) override;
        float getSpeedFactor() const override;

    public:
        float getTime() const override;

    protected:
        void update( const UpdateContext * _context ) override;

    protected:
        float m_speedFactor;
        float m_time;

        enum ESchdulerType
        {
            EST_EVENT,
            EST_TIMING
        };

        struct SchedulerEventDesc
        {
            UniqueId id;

            SchedulerEventInterfacePtr event;
            SchedulerTimingInterfacePtr timer;
            SchedulerPipeInterfacePtr pipe;

            float delay;
            float time_delay;

            uint32_t iterate;

            ESchdulerType type;
            bool dead;
            bool freeze;
            bool iterate_invalid;

#if defined(MENGINE_DOCUMENT_ENABLE)
            DocumentInterfacePtr doc;
#endif
        };

        typedef Vector<SchedulerEventDesc> VectorSchedulers;
        VectorSchedulers m_schedulersAdd;
        VectorSchedulers m_schedulers;

        bool m_freezeAll;
        bool m_update;

    protected:
        bool removeScheduler_( SchedulerEventDesc * const _event, bool _callStop );
        bool findSchedulerEvent_( UniqueId _id, const SchedulerEventDesc ** const _desc ) const;
        bool findSchedulerEvent_( UniqueId _id, SchedulerEventDesc ** const _desc );
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Scheduler, SchedulerInterface> SchedulerPtr;
    //////////////////////////////////////////////////////////////////////////
}
