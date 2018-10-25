#pragma once

#include "Interface/EventationInterface.h"

#include "Kernel/Factorable.h"
#include "Kernel/ConstString.h"
#include "Kernel/Mixin.h"

#include "Config/Vector.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class BaseEventation
        : public EventationInterface
    {
    public:
        BaseEventation() noexcept;
        ~BaseEventation() noexcept override;

    public:
        bool registerEventReceiver( uint32_t _event, const EventReceiverPtr & _receiver ) override;
        void removeEventReceiver( uint32_t _event ) override;

    public:
        const EventReceiverPtr & getEventReciever( uint32_t _event ) const override;
        bool hasEventReceiver( uint32_t _event ) const override;

    public:
        void removeEvents() noexcept override;

    protected:
        struct EventReceiverDesc
        {
            uint32_t event;
            EventReceiverPtr receiver;
        };

        typedef Vector<EventReceiverDesc> VectorEventReceivers;
        VectorEventReceivers m_receivers;

        uint64_t m_flag;

    private:
        class FEventReciver;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<BaseEventation> BaseEventPtr;
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {        
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        inline static bool hasEventableReceiver( T * _self, uint32_t _event )
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
        inline static bool hasEventableReceiver( const IntrusivePtr<T> & _self, uint32_t _event )
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
        static typename T::EventReceiverType * getThisEventRecieverT( T * _self, uint32_t _event )
        {
            typedef typename T::EventReceiverType * T_EventReceiverTypePtr;

            EventationInterface * eventation = _self->getEventation();

            T_EventReceiverTypePtr reciever = eventation->getEventRecieverT<T_EventReceiverTypePtr>( _event );

            return reciever;
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        static typename T::EventReceiverType * getThisEventRecieverT( const IntrusivePtr<T> & _self, uint32_t _event )
        {
            typedef typename T::EventReceiverType * T_EventReceiverTypePtr;

            EventationInterface * eventation = _self->getEventation();

            T_EventReceiverTypePtr reciever = eventation->getEventRecieverT<T_EventReceiverTypePtr>( _event );

            return reciever;
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        static T * getThisEventReciever( EventationInterface * _self, uint32_t _event )
        {
            T * reciever = _self->getEventRecieverT<T *>( _event );

            return reciever;
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        static T * getThisEventReciever( const EventationInterfacePtr & _self, uint32_t _event )
        {
            T * reciever = _self->getEventRecieverT<T *>( _event );

            return reciever;
        }
    }
}
//////////////////////////////////////////////////////////////////////////
#define EVENTABLE_METHODR(Self, Event, R)\
    Self == nullptr ? R : Helper::hasEventableReceiver( Self, Event ) == false ? R : Helper::getThisEventRecieverT( Self, Event )

#define EVENTABLE_METHOD(Self, Event)\
    EVENTABLE_METHODR(Self, Event, ((void)0))

#define EVENTABLE_METHODRS(Self, Event, R)\
    Helper::hasEventableReceiver( Self, Event ) == false ? R : Helper::getThisEventRecieverT( Self, Event )

#define EVENTABLE_METHODRT(Self, Event, R, Type)\
    Self == nullptr ? R : Helper::hasEventableReceiver( Self, Event ) == false ? R : Helper::getThisEventReciever<Type>( Self, Event )

#define EVENTABLE_METHODT(Self, Event, Type)\
    EVENTABLE_METHODRT(Self, Event, ((void)0), Type)

#define EVENTABLE_METHODRTS(Self, Event, R, Type)\
    Helper::hasEventableReceiver( Self, Event ) == false ? R : Helper::getThisEventReciever<Type>( Self, Event )


