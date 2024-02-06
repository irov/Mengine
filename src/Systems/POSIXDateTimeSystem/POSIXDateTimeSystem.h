#pragma once

#include "Interface/DateTimeSystemInterface.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
    class POSIXDateTimeSystem
        : public ServiceBase<DateTimeSystemInterface>
    {
    public:
        POSIXDateTimeSystem();
        ~POSIXDateTimeSystem() override;

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
