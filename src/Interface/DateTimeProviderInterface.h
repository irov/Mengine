#pragma once

#include "Interface/ServantInterface.h"

#include "Kernel/DateTime.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class DateTimeProviderInterface
        : public ServantInterface
    {
    public:
        virtual void getLocalDateTime( PlatformDateTime * const _dateTime ) const = 0;

    public:
        virtual uint64_t getLocalDateMilliseconds() const = 0;

    public:
        virtual void getLocalDateTimeFromMilliseconds( uint64_t _timestamp, PlatformDateTime * const _dateTime ) const = 0;

    public:
        virtual int32_t getTimeZoneOffset() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DateTimeProviderInterface> DateTimeProviderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}