#pragma once

#include "Interface/DateTimeProviderInterface.h"

namespace Mengine
{
    class Win32DateTimeProvider
        : public DateTimeProviderInterface
    {
    protected:
        void getLocalDateTime( PlatformDateTime * _dateTime ) const override;

    protected:
        uint64_t getLocalDateMilliseconds() const override;

    protected:
        int32_t getTimeZoneOffset() const override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Win32DateTimeProvider, DateTimeProviderInterface> Win32DateTimeProviderPtr;
    //////////////////////////////////////////////////////////////////////////
}