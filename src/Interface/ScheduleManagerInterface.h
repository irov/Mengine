#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/ServantInterface.h"
#include "Interface/UpdationInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/ConstString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ScheduleEventInterface
        : public Factorable
    {
    public:
        virtual void onScheduleComplete( uint32_t _id ) = 0;
        virtual void onScheduleStop( uint32_t _id ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ScheduleEventInterface> ScheduleEventInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class ScheduleTimingInterface
        : public Factorable
    {
    public:
        virtual void onScheduleUpdate( uint32_t _id, uint32_t _iterate, float _time ) = 0;
        virtual void onScheduleComplete( uint32_t _id ) = 0;
        virtual void onScheduleStop( uint32_t _id ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ScheduleTimingInterface> ScheduleTimerInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class SchedulePipeInterface
        : public Factorable
    {
    public:
        virtual float onSchedulePipe( uint32_t _id, uint32_t _index ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SchedulePipeInterface> SchedulePipeInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class ScheduleManagerInterface
        : public ServantInterface
    {
    public:
        virtual bool initialize( const ConstString & _name ) = 0;
        virtual void finalize() = 0;

    public:
        virtual const ConstString & getName() const = 0;

    public:
        virtual uint32_t event( float _delay, const ScheduleEventInterfacePtr & _listener ) = 0;

    public:
        virtual uint32_t timing( const SchedulePipeInterfacePtr & _pipe, const ScheduleTimerInterfacePtr & _listener ) = 0;

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
    typedef IntrusivePtr<ScheduleManagerInterface> ScheduleManagerInterfacePtr;
}