#pragma once

#include "Interface/DateTimeProviderInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class SDLDateTimeProvider
        : public DateTimeProviderInterface
    {
    public:
        SDLDateTimeProvider();
        ~SDLDateTimeProvider() override;

    protected:
        void getLocalDateTime( PlatformDateTime * _dateTime ) const override;

    public:
        uint64_t getLocalDateMilliseconds() const override;

    public:
        int32_t getTimeZoneOffset() const override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SDLDateTimeProvider, DateTimeProviderInterface> SDLDateTimeProviderPtr;
    //////////////////////////////////////////////////////////////////////////
}