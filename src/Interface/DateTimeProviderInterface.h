#pragma once

#include "Interface/ServantInterface.h"

#include "Kernel/DateTime.h"

namespace Mengine
{
    class DateTimeProviderInterface
        : public ServantInterface
    {
    public:
        virtual void getDateTime( PlatformDateTime * _dateTime ) const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<DateTimeProviderInterface> DateTimeProviderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}