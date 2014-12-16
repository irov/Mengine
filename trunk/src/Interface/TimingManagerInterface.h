#   pragma once

#   include "Interface/ServiceInterface.h"

#   include "Factory/FactorablePtr.h"

namespace Menge
{
    class TimingListenerInterface
		: public FactorablePtr
    {
    public:
        virtual bool updateTiming( uint32_t _id, float _timing ) = 0;
        virtual void removeTiming( uint32_t _id ) = 0;
    };

	typedef stdex::intrusive_ptr<TimingListenerInterface> TimingListenerInterfacePtr;

    class TimingManagerInterface
        : public Factorable
    {
    public:
        virtual void initialize( ServiceProviderInterface * _serviceProvider ) = 0;

    public:
        virtual uint32_t timing( float _delay, const TimingListenerInterfacePtr & _listener ) = 0;

    public:
        virtual bool remove( uint32_t _id ) = 0;
        virtual void removeAll() = 0;

        virtual void freeze( uint32_t _id, bool _freeze ) = 0;
        virtual void freezeAll( bool _freeze ) = 0;

        virtual bool isFreeze( uint32_t _id ) const = 0;

    public:
        virtual void update( float _current, float _timing ) = 0;
    };
}