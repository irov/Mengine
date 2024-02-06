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

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    protected:
        void getSystemDateTime( PlatformDateTime * const _dateTime ) const override;
        void getLocalDateTime( PlatformDateTime * const _dateTime ) const override;

    protected:
        void getDateTimeFromMilliseconds( Timestamp _time, PlatformDateTime * const _dateTime ) const override;

    protected:
        int64_t getTimeZoneOffsetMilliseconds() const override;

    protected:
        int64_t m_timeZoneOffset;
    };
}
