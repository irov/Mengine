#pragma once

#include "Interface/EventationInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/ConstString.h"
#include "Kernel/Mixin.h"

#include "Config/Vector.h"

#include <type_traits>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Eventable
        : public Mixin
    {
    public:
        virtual EventationInterface * getEventation()
        {
            return nullptr;
        };

        virtual const EventationInterface * getEventation() const
        {
            return nullptr;
        };
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Eventable> EventablePtr;
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        MENGINE_INLINE static bool hasEventableReceiver( const T * _self, uint32_t _event )
        {
            const EventationInterface * eventation = _self->getEventation();

            if( eventation == nullptr )
            {
                return false;
            }

            bool exist = eventation->hasEventReceiver( _event );

            return exist;
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        MENGINE_INLINE static bool hasEventableReceiver( const IntrusivePtr<T> & _self, uint32_t _event )
        {
            EventationInterface * eventation = _self->getEventation();

            if( eventation == nullptr )
            {
                return false;
            }

            bool exist = eventation->hasEventReceiver( _event );

            return exist;
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        static MENGINE_INLINE T * getThisEventReciever( Eventable * _self, uint32_t _event )
        {
            EventationInterface * eventation = _self->getEventation();

            T * reciever = eventation->getEventRecieverT<T *>( _event );

            return reciever;
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        static MENGINE_INLINE T * getThisEventReciever( const EventablePtr & _self, uint32_t _event )
        {
            EventationInterface * eventation = _self->getEventation();

            T * reciever = eventation->getEventRecieverT<T *>( _event );

            return reciever;
        }
    }
}
//////////////////////////////////////////////////////////////////////////
#define DECLARE_EVENTABLE()\
public:\
    Mengine::EventationInterface * getEventation() override{ return this; }\
    const Mengine::EventationInterface * getEventation() const override{ return this; }\
protected:
//////////////////////////////////////////////////////////////////////////
#define EVENTABLE_METHODRT(Event, R, Type)\
    Mengine::Helper::hasEventableReceiver( this, Event ) == false ? R : Mengine::Helper::getThisEventReciever<Type>( this, Event )
//////////////////////////////////////////////////////////////////////////
#define EVENTABLE_METHODR(Event, R)\
    EVENTABLE_METHODRT(Event, R, EVENTATION_GETTYPE(Event))
//////////////////////////////////////////////////////////////////////////
#define EVENTABLE_METHOD(Event)\
    EVENTABLE_METHODRT(Event, MENGINE_UNUSED(0), EVENTATION_GETTYPE(Event))
//////////////////////////////////////////////////////////////////////////
#define EVENTABLE_OTHER_METHODRT(Self, Event, R, Type)\
    Self == nullptr ? R : Mengine::Helper::hasEventableReceiver( Self, Event ) == false ? R : Mengine::Helper::getThisEventReciever<Type>( Self, Event )
//////////////////////////////////////////////////////////////////////////
#define EVENTABLE_OTHER_METHODR(Self, Event, R)\
    EVENTABLE_OTHER_METHODRT(Self, Event, R, EVENTATION_GETTYPE(Event))
//////////////////////////////////////////////////////////////////////////
#define EVENTABLE_OTHER_METHOD(Self, Event)\
    EVENTABLE_OTHER_METHODRT(Self, Event, MENGINE_UNUSED(0), EVENTATION_GETTYPE(Event))
//////////////////////////////////////////////////////////////////////////
#define EVENTABLE_OTHER_METHODRT(Self, Event, R, Type)\
    Self == nullptr ? R : Mengine::Helper::hasEventableReceiver( Self, Event ) == false ? R : Mengine::Helper::getThisEventReciever<Type>( Self, Event )
//////////////////////////////////////////////////////////////////////////
#define EVENTABLE_OTHER_METHODT(Self, Event, Type)\
    EVENTABLE_OTHER_METHODRT(Self, Event, MENGINE_UNUSED(0), Type)
//////////////////////////////////////////////////////////////////////////