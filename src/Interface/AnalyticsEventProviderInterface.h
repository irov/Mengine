#pragma once

#include "Interface/Interface.h"
#include "Interface/AnalyticsEventInterface.h"

#include "Kernel/ConstString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AnalyticsEventProviderInterface
        : public Interface
    {
    public:
        virtual void onAnalyticsEvent( const AnalyticsEventInterfacePtr & _event ) = 0;
        virtual void onAnalyticsScreenView( const ConstString & _screenType, const ConstString & _screenName ) = 0;

    public:
        virtual void onAnalyticsFlush() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AnalyticsEventProviderInterface> AnalyticsEventProviderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}