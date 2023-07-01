#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/AnalyticsEventProviderInterface.h"
#include "Interface/AnalyticsEventBuilderInterface.h"
#include "Interface/AnalyticsEventInterface.h"
#include "Interface/AnalyticsContextInterface.h"

#include "Kernel/ConstString.h"
#include "Kernel/String.h"
#include "Kernel/Document.h"

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

    public:
        virtual void logEarnVirtualCurrency( const ConstString & _currencyName, double _value, const DocumentPtr & _doc ) = 0;
        virtual void logSpendVirtualCurrency( const ConstString & _itemName, const ConstString & _currencyName, double _value, const DocumentPtr & _doc ) = 0;
        virtual void logUnlockAchievement( const ConstString & _achievementId, const DocumentPtr & _doc ) = 0;
        virtual void logLevelUp( const ConstString & _character, int64_t _level, const DocumentPtr & _doc ) = 0;
        virtual void logLevelStart( const ConstString & _name, const DocumentPtr & _doc ) = 0;
        virtual void logLevelEnd( const ConstString & _name, bool _successful, const DocumentPtr & _doc ) = 0;
        virtual void logSelectItem( const ConstString & _category, const ConstString & _itemId, const DocumentPtr & _doc ) = 0;
        virtual void logTutorialBegin( const DocumentPtr & _doc ) = 0;
        virtual void logTutorialComplete( const DocumentPtr & _doc ) = 0;

    public:
        virtual AnalyticsEventBuilderInterfacePtr buildEvent( const ConstString & _eventName, const DocumentPtr & _doc ) = 0;

    public:
        virtual AnalyticsContextInterfacePtr makeContext( const DocumentPtr & _doc ) = 0;
    
    public:
        virtual const AnalyticsContextInterfacePtr & getGlobalContext() const = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define ANALYTICS_SERVICE()\
    ((Mengine::AnalyticsServiceInterface *)SERVICE_GET(Mengine::AnalyticsServiceInterface))
//////////////////////////////////////////////////////////////////////////