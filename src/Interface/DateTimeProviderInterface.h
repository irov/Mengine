#pragma once

#include "Interface/Interface.h"

#include "Kernel/DateTime.h"

namespace Mengine
{
    class DateTimeProviderInterface
        : public Interface
    {
    public:
        virtual void getDateTime( PlatformDateTime * _dateTime ) const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DateTimeProviderInterface> DateTimeProviderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}