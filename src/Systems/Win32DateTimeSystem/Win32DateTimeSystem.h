#pragma once

#include "Interface/DateTimeSystemInterface.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
    class Win32DateTimeSystem
        : public ServiceBase<DateTimeSystemInterface>
    {
    public:
        Win32DateTimeSystem();
        ~Win32DateTimeSystem() override;

    protected:
        void getLocalDateTime( PlatformDateTime * const _dateTime ) const override;

    protected:
        uint64_t getLocalDateMilliseconds() const override;

    protected:
        void getLocalDateTimeFromMilliseconds( uint64_t _timestamp, PlatformDateTime * const _dateTime ) const override;

    protected:
        int32_t getTimeZoneOffset() const override;
    };
}
