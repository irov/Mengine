#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/AnalyticsEventProviderInterface.h"
#include "Interface/AnalyticsEventBuilderInterface.h"
#include "Interface/AnalyticsEventInterface.h"
#include "Interface/AnalyticsContextInterface.h"
#include "Interface/DocumentInterface.h"

#include "Kernel/ConstString.h"
#include "Kernel/String.h"

#include "Config/Timestamp.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AnalyticsServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "AnalyticsService" )

    public:
        virtual void addEventProvider( const AnalyticsEventProviderInterfacePtr & _provider ) = 0;
        virtual void removeEventProvider( const AnalyticsEventProviderInterfacePtr & _provider ) = 0;

    public:
        virtual void logEvent( const AnalyticsEventInterfacePtr & _event ) = 0;
        virtual void logScreenView( const ConstString & _screenType, const ConstString & _screenName ) = 0;
        virtual void logFlush() = 0;

    public:
        virtual AnalyticsEventBuilderInterfacePtr buildEvent( EAnalyticsEventCategory _category, const ConstString & _eventName, const DocumentInterfacePtr & _doc ) = 0;

    public:
        virtual AnalyticsContextInterfacePtr makeContext( const DocumentInterfacePtr & _doc ) = 0;
    
    public:
        virtual const AnalyticsContextInterfacePtr & getGlobalContext() const = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define ANALYTICS_SERVICE()\
    ((Mengine::AnalyticsServiceInterface *)SERVICE_GET(Mengine::AnalyticsServiceInterface))
//////////////////////////////////////////////////////////////////////////