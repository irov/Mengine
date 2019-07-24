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
        template<class T, class EventReceiverType = std::conditional_t<std::is_const_v<T>, const typename T::EventReceiverType *, typename T::EventReceiverType *>>
        static MENGINE_INLINE EventReceiverType getThisEventRecieverT( T * _self, uint32_t _event )
        {
            EventationInterface * eventation = _self->getEventation();

            EventReceiverType reciever = eventation->getEventRecieverT<EventReceiverType>( _event );

            return reciever;
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        static MENGINE_INLINE typename T::EventReceiverType * getThisEventRecieverT( const IntrusivePtr<T> & _self, uint32_t _event )
        {
            typedef typename T::EventReceiverType * T_EventReceiverTypePtr;

            EventationInterface * eventation = _self->getEventation();

            T_EventReceiverTypePtr reciever = eventation->getEventRecieverT<T_EventReceiverTypePtr>( _event );

            return reciever;
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
#define DECLARE_EVENTABLE_TYPE( Type )\
public:\
    typedef Type EventReceiverType;
//////////////////////////////////////////////////////////////////////////
#define DECLARE_EVENTABLE( Type )\
public:\
    DECLARE_EVENTABLE_TYPE( Type );\
    Mengine::EventationInterface * getEventation() override{ return this; }\
    const Mengine::EventationInterface * getEventation() const override{ return this; }\
protected:
//////////////////////////////////////////////////////////////////////////
#define EVENTABLE_METHODR(Event, R)\
    Mengine::Helper::hasEventableReceiver( this, Event ) == false ? R : Mengine::Helper::getThisEventRecieverT( this, Event )
//////////////////////////////////////////////////////////////////////////
#define EVENTABLE_METHOD(Event)\
    EVENTABLE_METHODR(Event, MENGINE_UNUSED(0))
//////////////////////////////////////////////////////////////////////////
#define EVENTABLE_OTHER_METHODR(Self, Event, R)\
    Self == nullptr ? R : Mengine::Helper::hasEventableReceiver( Self, Event ) == false ? R : Mengine::Helper::getThisEventRecieverT( Self, Event )
//////////////////////////////////////////////////////////////////////////
#define EVENTABLE_OTHER_METHOD(Self, Event)\
    EVENTABLE_OTHER_METHODR(Self, Event, MENGINE_UNUSED(0))
//////////////////////////////////////////////////////////////////////////
#define EVENTABLE_METHODRS(Self, Event, R)\
    Mengine::Helper::hasEventableReceiver( Self, Event ) == false ? R : Mengine::Helper::getThisEventRecieverT( Self, Event )
//////////////////////////////////////////////////////////////////////////
#define EVENTABLE_OTHER_METHODRT(Self, Event, R, Type)\
    Self == nullptr ? R : Mengine::Helper::hasEventableReceiver( Self, Event ) == false ? R : Mengine::Helper::getThisEventReciever<Type>( Self, Event )
//////////////////////////////////////////////////////////////////////////
#define EVENTABLE_OTHER_METHODT(Self, Event, Type)\
    EVENTABLE_OTHER_METHODRT(Self, Event, MENGINE_UNUSED(0), Type)
//////////////////////////////////////////////////////////////////////////