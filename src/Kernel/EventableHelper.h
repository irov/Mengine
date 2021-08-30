#pragma once

#include "Interface/EventationInterface.h"

#include "Kernel/Eventable.h"

namespace Mengine
{
    namespace Helper
    {
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        bool hasEventableReceiver( const T * _self, uint32_t _event )
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
        bool hasEventableReceiver( const IntrusivePtr<T> & _self, uint32_t _event )
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
        T * getThisEventReceiver( Eventable * _self, uint32_t _event )
        {
            EventationInterface * eventation = _self->getEventation();

            T * receiver = eventation->getEventReceiverT<T *>( _event );

            return receiver;
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        T * getThisEventReceiver( const EventablePtr & _self, uint32_t _event )
        {
            EventationInterface * eventation = _self->getEventation();

            T * receiver = eventation->getEventReceiverT<T *>( _event );

            return receiver;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
//////////////////////////////////////////////////////////////////////////
#define EVENTABLE_METHODRT(Event, R, Type)\
    Mengine::Helper::hasEventableReceiver( this, Event ) == false ? R : Mengine::Helper::getThisEventReceiver<Type>( this, Event )
//////////////////////////////////////////////////////////////////////////
#define EVENTABLE_METHODR(Event, R)\
    EVENTABLE_METHODRT(Event, R, EVENTATION_GETTYPE(Event))
//////////////////////////////////////////////////////////////////////////
#define EVENTABLE_METHOD(Event)\
    EVENTABLE_METHODRT(Event, MENGINE_UNUSED(0), EVENTATION_GETTYPE(Event))
//////////////////////////////////////////////////////////////////////////
#define EVENTABLE_OTHER_METHODRT(Self, Event, R, Type)\
    Self == nullptr ? R : Mengine::Helper::hasEventableReceiver( Self, Event ) == false ? R : Mengine::Helper::getThisEventReceiver<Type>( Self, Event )
//////////////////////////////////////////////////////////////////////////
#define EVENTABLE_OTHER_METHODR(Self, Event, R)\
    EVENTABLE_OTHER_METHODRT(Self, Event, R, EVENTATION_GETTYPE(Event))
//////////////////////////////////////////////////////////////////////////
#define EVENTABLE_OTHER_METHOD(Self, Event)\
    EVENTABLE_OTHER_METHODRT(Self, Event, MENGINE_UNUSED(0), EVENTATION_GETTYPE(Event))
//////////////////////////////////////////////////////////////////////////
#define EVENTABLE_OTHER_METHODT(Self, Event, Type)\
    EVENTABLE_OTHER_METHODRT(Self, Event, MENGINE_UNUSED(0), Type)
//////////////////////////////////////////////////////////////////////////