#	pragma once

#   include "Interface/ServiceInterface.h"

#   include "Factory/FactorablePtr.h"
#	include "Kernel/EventReceiver.h"
#	include "Core/ConstString.h"

#   include "stdex/stl_vector.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	class Eventable
	{
	public:
		Eventable();
		~Eventable();

    public:
        bool registerEventReceiver(uint32_t _event, const EventReceiverPtr & _receiver );
        void removeEventReceiver( uint32_t _event );

    public:
        const EventReceiverPtr & getEventReciever( uint32_t _event ) const;
        bool hasEventReceiver( uint32_t _event ) const;

    public:
        void removeEvents();

    public:
        template<class T>
        T getEventRecieverT( uint32_t _event ) const
        {   
            const EventReceiverPtr & reciever = this->getEventReciever( _event );

            if( reciever == nullptr )
            {
                return nullptr;
            }

            EventReceiver * r = reciever.get();
            
#   ifdef _DEBUG            
            if( dynamic_cast<T>(r) == nullptr )
            {
                throw;
            }
#   endif

            T t = static_cast<T>(r);

            return t;
        }


	protected:
        struct EventReceiverDesc
        {
            uint32_t event;
            EventReceiverPtr receiver;
        };

        typedef stdex::vector<EventReceiverDesc> TMapEventReceivers;
        TMapEventReceivers m_receivers;

        uint64_t m_flag;

    private:
        class FEventReciver;
	};
    //////////////////////////////////////////////////////////////////////////
    namespace Helper
    {
        template<class T>
        static typename T::EventReceiverType * getThisEventRecieverT( T * _self, uint32_t _event )
        {
            typename T::EventReceiverType * reciever = _self->getEventRecieverT<typename T::EventReceiverType *>( _event );

            return reciever;
        }

        template<class T>
        static typename T * getThisEventReciever( Eventable * _self, uint32_t _event )
        {
            typename T * reciever = _self->getEventRecieverT<T *>( _event );

            return reciever;
        }
    }
}

#   define EVENT_RECEIVER(Type)\
public:\
    typedef Type EventReceiverType

#   define EVENTABLE_METHODR(Self, Event, R)\
    Self == nullptr ? R : Self->hasEventReceiver( Event ) == false ? R : Helper::getThisEventRecieverT( Self, Event )

#   define EVENTABLE_METHOD(Self, Event)\
    EVENTABLE_METHODR(Self, Event, ((void)0))

#   define EVENTABLE_METHODRS(Self, Event, R)\
    Self->hasEventReceiver( Event ) == false ? R : Helper::getThisEventRecieverT( Self, Event )

#   define EVENTABLE_METHODRT(Self, Event, R, Type)\
    Self == nullptr ? R : Self->hasEventReceiver( Event ) == false ? R : Helper::getThisEventReciever<Type>( Self, Event )

#   define EVENTABLE_METHODT(Self, Event, Type)\
    EVENTABLE_METHODRT(Self, Event, ((void)0), Type)

#   define EVENTABLE_METHODRTS(Self, Event, R, Type)\
    Self->hasEventReceiver( Event ) == false ? R : Helper::getThisEventReciever<Type>( Self, Event )


