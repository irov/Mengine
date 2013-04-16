#   pragma once

#   include "Interface/ServiceInterface.h"

#   include "Config/Typedef.h"
#   include "Factory/Factorable.h"

namespace Menge
{
    class TimingListener
    {
    public:
        virtual bool updateTiming( size_t _id, float _timing ) = 0;
        virtual void removeTiming( size_t _id ) = 0;

    public:
        virtual void deleteTimingListener() const = 0;
    };

    class TimingManagerInterface
        : public Factorable
    {
    public:
        virtual void initialize( ServiceProviderInterface * _serviceProvider ) = 0;

    public:
        virtual size_t timing( bool _portions, bool _global, float _delay, TimingListener * _listener ) = 0;

    public:
        virtual void remove( size_t _timingID ) = 0;
        virtual void removeAll( bool _global ) = 0;

        virtual void freeze( size_t _id, bool _freeze ) = 0;
        virtual void freezeAll( bool _freeze ) = 0;

        virtual bool isFreeze( size_t _id ) const = 0;

    public:
        virtual void update( float _current, float _timing ) = 0;
    };
}