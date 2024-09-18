#pragma once

#include "Interface/ServiceInterface.h"

#include "Kernel/DateTime.h"

#include "Config/Timestamp.h"

namespace Mengine
{
    class DateTimeSystemInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "DateTimeSystem" )

    public:
        virtual void getSystemDateTime( PlatformDateTime * const _dateTime ) const = 0;
        virtual void getLocalDateTime( PlatformDateTime * const _dateTime ) const = 0;

    public:
        virtual void getDateTimeFromMilliseconds( Timestamp _time, PlatformDateTime * const _dateTime ) const = 0;

    public:
        virtual int64_t getTimeZoneOffsetMilliseconds() const = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define DATETIME_SYSTEM()\
    ((Mengine::DateTimeSystemInterface *)SERVICE_GET(Mengine::DateTimeSystemInterface))
//////////////////////////////////////////////////////////////////////////