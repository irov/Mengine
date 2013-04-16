#   pragma once

#   include "Interface/ServiceInterface.h"

#   include "Config/Typedef.h"
#   include "Factory/Factorable.h"

namespace Menge
{
    class ScheduleListener
    {
    public:
        virtual void onScheduleComplete( size_t _id ) = 0;
        virtual void onScheduleStop( size_t _id ) = 0;

    public:
        virtual void destroyScheduleListener() const = 0;
    };

    class ScheduleManagerInterface
        : public Factorable
    {
    public:
        virtual void initialize( ServiceProviderInterface * _serviceProvider ) = 0;

    public:
        virtual size_t schedule( float _timing, ScheduleListener * _func ) = 0;

        virtual void remove( size_t _id ) = 0;
        virtual void removeAll() = 0;

        virtual void freeze( size_t _id, bool _freeze ) = 0;
        virtual void freezeAll( bool _freeze ) = 0;
        virtual bool isFreeze( size_t _id ) const = 0;

        virtual float time( size_t _id ) const = 0;

    public:
        virtual void update( float _current, float _timing ) = 0;
    };
}