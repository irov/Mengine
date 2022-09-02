#pragma once

#include "Interface/DateTimeProviderInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class SDLDateTimeProvider
        : public DateTimeProviderInterface
    {
        DECLARE_FACTORABLE( SDLDateTimeProvider );

    public:
        SDLDateTimeProvider();
        ~SDLDateTimeProvider() override;

    protected:
        void getLocalDateTime( PlatformDateTime * const _dateTime ) const override;

    public:
        uint64_t getLocalDateMilliseconds() const override;

    public:
        void getLocalDateTimeFromMilliseconds( uint64_t _timestamp, PlatformDateTime * const _dateTime ) const override;

    public:
        int32_t getTimeZoneOffset() const override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SDLDateTimeProvider, DateTimeProviderInterface> SDLDateTimeProviderPtr;
    //////////////////////////////////////////////////////////////////////////
}