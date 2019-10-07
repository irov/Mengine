#pragma once

#include "Interface/DateTimeProviderInterface.h"

namespace Mengine
{
    class Win32DateTimeProvider
        : public DateTimeProviderInterface
    {
    protected:
        void getDateTime( PlatformDateTime * _dateTime ) const override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Win32DateTimeProvider, DateTimeProviderInterface> Win32DateTimeProviderPtr;
    //////////////////////////////////////////////////////////////////////////
}