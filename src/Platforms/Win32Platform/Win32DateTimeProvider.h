#pragma once

#include "Interface/DateTimeProviderInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class Win32DateTimeProvider
        : public DateTimeProviderInterface
        , public Factorable
    {
    protected:
        void getDateTime( PlatformDateTime * _dateTime ) const override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Win32DateTimeProvider, DateTimeProviderInterface> Win32DateTimeProviderPtr;
    //////////////////////////////////////////////////////////////////////////
}