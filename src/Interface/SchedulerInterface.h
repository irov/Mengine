#pragma once

#include "Interface/ServantInterface.h"
#include "Interface/ServiceInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/ConstString.h"

#include "Config/UniqueId.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class SchedulerEventInterface
        : public Interface
    {
    public:
        virtual void onSchedulerComplete( UniqueId _id ) = 0;
        virtual void onSchedulerStop( UniqueId _id ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SchedulerEventInterface> SchedulerEventInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class SchedulerTimingInterface
        : public Interface
    {
    public:
        virtual void onSchedulerTiming( UniqueId _id, uint32_t _iterate, float _time ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SchedulerTimingInterface> SchedulerTimingInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class SchedulerPipeInterface
        : public Interface
    {
    public:
        virtual float onSchedulerPipe( UniqueId _id, uint32_t _iterate ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SchedulerPipeInterface> SchedulerPipeInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class SchedulerInterface
        : public ServantInterface
    {
    public:
        virtual bool initialize() = 0;
        virtual void finalize() = 0;

    public:
        virtual UniqueId event( float _delay, const SchedulerEventInterfacePtr & _event, const DocumentInterfacePtr & _doc ) = 0;

    public:
        virtual UniqueId timing( const SchedulerPipeInterfacePtr & _pipe, const SchedulerTimingInterfacePtr & _timing, const SchedulerEventInterfacePtr & _event, const DocumentInterfacePtr & _doc ) = 0;

    public:
        virtual bool refresh( UniqueId _id ) = 0;

    public:
        virtual bool exist( UniqueId _id ) const = 0;

    public:
        virtual bool remove( UniqueId _id ) = 0;
        virtual void removeAll() = 0;

        virtual bool cancel( UniqueId _id ) = 0;
        virtual void cancelAll() = 0;

        virtual bool freeze( UniqueId _id, bool _freeze ) = 0;
        virtual void freezeAll( bool _freeze ) = 0;
        virtual bool isFreezeAll() const = 0;
        virtual bool isFreeze( UniqueId _id ) const = 0;

        virtual float getTimePassed( UniqueId _id ) const = 0;
        virtual float getTimeLeft( UniqueId _id ) const = 0;

    public:
        virtual void setSpeedFactor( float _factor ) = 0;
        virtual float getSpeedFactor() const = 0;

    public:
        virtual float getTime() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SchedulerInterface> SchedulerInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}