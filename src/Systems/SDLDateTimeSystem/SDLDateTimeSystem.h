#pragma once

#include "Interface/DateTimeSystemInterface.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
    class SDLDateTimeSystem
        : public ServiceBase<DateTimeSystemInterface>
    {
    public:
        SDLDateTimeSystem();
        ~SDLDateTimeSystem() override;

    protected:
        void getLocalDateTime( PlatformDateTime * const _dateTime ) const override;

    protected:
        TimeMilliseconds getLocalDateTimeMilliseconds() const override;

    protected:
        void getLocalDateTimeFromMilliseconds( TimeMilliseconds _time, PlatformDateTime * const _dateTime ) const override;

    protected:
        int32_t getTimeZoneOffset() const override;
    };
}
