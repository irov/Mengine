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
        AnalyticsEventBuilderInterfacePtr buildEvent( const ConstString & _eventName ) override;

    public:
        const AnalyticsContextInterfacePtr & getGlobalContext() const override;

    protected:
        typedef Vector<AnalyticsEventProviderInterfacePtr> VectorAnalyticsEventProvider;
        VectorAnalyticsEventProvider m_eventProviders;

        AnalyticsFactoryInterfacePtr m_analyticsFactory;

        AnalyticsContextInterfacePtr m_analyticsGlobalContext;

        FactoryInterfacePtr m_factoryAnalyticsEventBuilder;
    };
}
