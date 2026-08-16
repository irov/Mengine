#pragma once

#include "Interface/MetricServiceInterface.h"
#include "Interface/ThreadSharedMutexInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Vector.h"

namespace Mengine
{
    class MetricService
        : public ServiceBase<MetricServiceInterface>
    {
        DECLARE_FACTORABLE( MetricService );

    public:
        MetricService();
        ~MetricService() override;

    public:
        const ServiceRequiredList & requiredServices() const override;
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        bool registerMetric( const MetricDesc & _desc, const DocumentInterfacePtr & _doc ) override;
        bool unregisterMetric( const ConstString & _name, const ConstString & _owner ) override;
        uint32_t unregisterMetrics( const ConstString & _owner ) override;
        bool hasMetric( const ConstString & _name ) const override;
        bool setMetricEnabled( const ConstString & _name, bool _enabled ) override;

    public:
        bool setMetricInteger( const ConstString & _name, int64_t _value ) override;
        bool addMetricInteger( const ConstString & _name, int64_t _value ) override;
        bool setMetricDouble( const ConstString & _name, double _value ) override;
        bool addMetricDouble( const ConstString & _name, double _value ) override;
        bool setMetricString( const ConstString & _name, const String & _value ) override;
        bool recordMetricSample( const ConstString & _name, double _value ) override;

    public:
        bool getMetric( const ConstString & _name, MetricSnapshot * const _snapshot ) const override;
        void foreachMetrics( const LambdaMetric & _lambda ) const override;

    protected:
        struct MetricEntry
        {
            MetricSnapshot snapshot;

#if defined(MENGINE_DOCUMENT_ENABLE)
            DocumentInterfacePtr doc;
#endif
        };

        typedef Vector<MetricEntry> VectorMetrics;

    protected:
        MetricEntry * findMetric_( const ConstString & _name );
        const MetricEntry * findMetric_( const ConstString & _name ) const;
        bool validateMetricDesc_( const MetricDesc & _desc ) const;

    protected:
        ThreadSharedMutexInterfacePtr m_mutex;
        VectorMetrics m_metrics;
    };
}
