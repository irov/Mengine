#pragma once

#include "Interface/ServantInterface.h"

#include "Kernel/DateTime.h"

namespace Mengine
{
    class DateTimeProviderInterface
        : public ServantInterface
    {
    public:
        virtual void getLocalDateTime( PlatformDateTime * _dateTime ) const = 0;

    public:
        virtual uint64_t getLocalDateMilliseconds() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DateTimeProviderInterface> DateTimeProviderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}