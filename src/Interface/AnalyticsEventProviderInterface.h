#pragma once

#include "Interface/Interface.h"
#include "Interface/AnalyticsEventInterface.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AnalyticsEventProviderInterface
        : public Interface
    {
    public:
        virtual void onAnalyticsEvent( const AnalyticsEventInterfacePtr & _event ) = 0;
        virtual void onAnalyticsScreenView( const ConstString & _screenType, const ConstString & _screenName ) = 0;

        virtual void onAnalyticsFlush() = 0;        
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AnalyticsEventProviderInterface> AnalyticsEventProviderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}