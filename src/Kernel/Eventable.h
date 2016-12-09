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
        bool registerEventReceiver( uint32_t _event, const EventReceiverPtr & _receiver );
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

		uint32_t m_flag;

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
    }
}

#   define EVENT_RECEIVER(Type)\
public:\
    typedef Type EventReceiverType

#   define EVENTABLE_METHOD(Self, Event)\
    Self->hasEvent( Event ) == false ? (void)0 : Helper::getThisEventRecieverT( Self, Event )

#   define EVENTABLE_METHODR(Self, Event, R)\
    Self->hasEvent( Event ) == false ? R : Helper::getThisEventRecieverT( Self, Event )

