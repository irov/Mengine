#pragma once

#include "Interface/AnalyticsServiceInterface.h"
#include "Interface/FactoryInterface.h"

#include "AnalyticsFactoryInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Vector.h"

namespace Mengine
{
    class AnalyticsService
        : public ServiceBase<AnalyticsServiceInterface>
    {
    public:
        AnalyticsService();
        ~AnalyticsService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        void addEventProvider( const AnalyticsEventProviderInterfacePtr & _provider ) override;
        void removeEventProvider( const AnalyticsEventProviderInterfacePtr & _provider ) override;

    public:
        void logEvent( const AnalyticsEventInterfacePtr & _event ) override;

    public:
        void logEarnVirtualCurrency( const ConstString & _currencyName, double _value, const DocumentInterfacePtr & _doc ) override;
        void logSpendVirtualCurrency( const ConstString & _itemName, const ConstString & _currencyName, double _value, const DocumentInterfacePtr & _doc ) override;
        void logUnlockAchievement( const ConstString & _achievementId, const DocumentInterfacePtr & _doc ) override;
        void logLevelUp( const ConstString & _character, int64_t _level, const DocumentInterfacePtr & _doc ) override;
        void logLevelStart( const ConstString & _name, const DocumentInterfacePtr & _doc ) override;
        void logLevelEnd( const ConstString & _name, bool _successful, const DocumentInterfacePtr & _doc ) override;
        void logSelectItem( const ConstString & _category, const ConstString & _itemId, const DocumentInterfacePtr & _doc ) override;
        void logTutorialBegin( const DocumentInterfacePtr & _doc ) override;
        void logTutorialComplete( const DocumentInterfacePtr & _doc ) override;

    public:
        AnalyticsEventBuilderInterfacePtr buildEvent( const ConstString & _eventName, const DocumentInterfacePtr & _doc ) override;

    public:
        AnalyticsContextInterfacePtr makeContext( const DocumentInterfacePtr & _doc ) override;

    public:
        const AnalyticsContextInterfacePtr & getGlobalContext() const override;

    protected:
        typedef Vector<AnalyticsEventProviderInterfacePtr> VectorAnalyticsEventProvider;
        VectorAnalyticsEventProvider m_eventProviders;

        AnalyticsFactoryInterfacePtr m_analyticsFactory;

        AnalyticsContextInterfacePtr m_analyticsGlobalContext;
    };
}
