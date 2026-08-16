#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/DocumentInterface.h"

#include "Kernel/ConstString.h"
#include "Kernel/String.h"

#include "Config/Lambda.h"
#include "Config/StdInt.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    enum class EMetricType : uint8_t
    {
        Counter,
        Gauge,
        Histogram
    };
    //////////////////////////////////////////////////////////////////////////
    enum class EMetricValueType : uint8_t
    {
        Integer,
        Double,
        String
    };
    //////////////////////////////////////////////////////////////////////////
    struct MetricDesc
    {
        ConstString name;
        ConstString owner;
        ConstString unit;
        EMetricType type;
        EMetricValueType valueType;
        bool enabled;
    };
    //////////////////////////////////////////////////////////////////////////
    struct MetricSnapshot
    {
        MetricDesc desc;
        int64_t integerValue;
        double doubleValue;
        String stringValue;
        uint64_t sampleCount;
        double sampleSum;
        double sampleMin;
        double sampleMax;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef Lambda<void( const MetricSnapshot & )> LambdaMetric;
    //////////////////////////////////////////////////////////////////////////
    class MetricServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "MetricService" )

    public:
        virtual bool registerMetric( const MetricDesc & _desc, const DocumentInterfacePtr & _doc ) = 0;
        virtual bool unregisterMetric( const ConstString & _name, const ConstString & _owner ) = 0;
        virtual uint32_t unregisterMetrics( const ConstString & _owner ) = 0;
        virtual bool hasMetric( const ConstString & _name ) const = 0;
        virtual bool setMetricEnabled( const ConstString & _name, bool _enabled ) = 0;

    public:
        virtual bool setMetricInteger( const ConstString & _name, int64_t _value ) = 0;
        virtual bool addMetricInteger( const ConstString & _name, int64_t _value ) = 0;
        virtual bool setMetricDouble( const ConstString & _name, double _value ) = 0;
        virtual bool addMetricDouble( const ConstString & _name, double _value ) = 0;
        virtual bool setMetricString( const ConstString & _name, const String & _value ) = 0;
        virtual bool recordMetricSample( const ConstString & _name, double _value ) = 0;

    public:
        virtual bool getMetric( const ConstString & _name, MetricSnapshot * const _snapshot ) const = 0;
        virtual void foreachMetrics( const LambdaMetric & _lambda ) const = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define METRIC_SERVICE()\
    ((Mengine::MetricServiceInterface *)SERVICE_GET(Mengine::MetricServiceInterface))
//////////////////////////////////////////////////////////////////////////
