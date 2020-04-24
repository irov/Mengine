#pragma once

#include "Interface/Interface.h"
#include "Interface/ServiceInterface.h"
#include "Interface/UpdationInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/ConstString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class SchedulerEventInterface
        : public Interface
    {
    public:
        virtual void onSchedulerComplete( uint32_t _id ) = 0;
        virtual void onSchedulerStop( uint32_t _id ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SchedulerEventInterface> SchedulerEventInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class SchedulerTimingInterface
        : public Interface
    {
    public:
        virtual void onSchedulerTiming( uint32_t _id, uint32_t _iterate, float _time ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SchedulerTimingInterface> SchedulerTimingInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class SchedulerPipeInterface
        : public Interface
    {
    public:
        virtual float onSchedulerPipe( uint32_t _id, uint32_t _index ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SchedulerPipeInterface> SchedulerPipeInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class SchedulerInterface
        : public Interface
    {
    public:
        virtual bool initialize() = 0;
        virtual void finalize() = 0;

    public:
        virtual void setName( const ConstString & _name ) = 0;
        virtual const ConstString & getName() const = 0;

    public:
        virtual uint32_t event( float _delay, const SchedulerEventInterfacePtr & _event, const DocumentPtr & _doc ) = 0;

    public:
        virtual uint32_t timing( const SchedulerPipeInterfacePtr & _pipe, const SchedulerTimingInterfacePtr & _timing, const SchedulerEventInterfacePtr & _event, const DocumentPtr & _doc ) = 0;

    public:
        virtual bool refresh( uint32_t _id ) = 0;

    public:
        virtual bool exist( uint32_t _id ) const = 0;

    public:
        virtual bool remove( uint32_t _id ) = 0;
        virtual void removeAll() = 0;

        virtual bool freeze( uint32_t _id, bool _freeze ) = 0;
        virtual void freezeAll( bool _freeze ) = 0;
        virtual bool isFreezeAll() const = 0;
        virtual bool isFreeze( uint32_t _id ) const = 0;

        virtual float getTimePassed( uint32_t _id ) const = 0;
        virtual float getTimeLeft( uint32_t _id ) const = 0;

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