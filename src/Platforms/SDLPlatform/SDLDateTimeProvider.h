#pragma once

#include "Interface/DateTimeProviderInterface.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    class SDLDateTimeProvider
        : public DateTimeProviderInterface
        , public Factorable
    {
    protected:
        void getDateTime( PlatformDateTime * _dateTime ) const override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<SDLDateTimeProvider, DateTimeProviderInterface> SDLDateTimeProviderPtr;
    //////////////////////////////////////////////////////////////////////////
}