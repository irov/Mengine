#include "MetricService.h"

#include "Interface/ThreadSystemInterface.h"

#include "Kernel/ThreadSharedMutexScope.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ThreadMutexHelper.h"
#include "Kernel/Logger.h"

#include "Config/StdAlgorithm.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( MetricService, Mengine::MetricService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MetricService::MetricService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MetricService::~MetricService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    const ServiceRequiredList & MetricService::requiredServices() const
    {
        static ServiceRequiredList required = {
            SERVICE_ID( ThreadSystemInterface )
        };

        return required;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetricService::_initializeService()
    {
        ThreadSharedMutexInterfacePtr mutex = Helper::createThreadSharedMutex( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( mutex, "invalid create shared mutex" );

        m_mutex = mutex;
        m_metrics.reserve( 32 );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetricService::_finalizeService()
    {
#if defined(MENGINE_DOCUMENT_ENABLE)
        for( const MetricEntry & metric : m_metrics )
        {
            LOGGER_ASSERTION( "forgot unregister metric '%s' owner '%s' (doc: %s)"
                , metric.snapshot.desc.name.c_str()
                , metric.snapshot.desc.owner.c_str()
                , MENGINE_DOCUMENT_STR( metric.doc )
            );
        }
#endif

        m_metrics.clear();
        m_mutex = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetricService::registerMetric( const MetricDesc & _desc, const DocumentInterfacePtr & _doc )
    {
        if( this->validateMetricDesc_( _desc ) == false )
        {
            return false;
        }

        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        if( this->findMetric_( _desc.name ) != nullptr )
        {
            LOGGER_ERROR( "metric '%s' already registered"
                , _desc.name.c_str()
            );

            return false;
        }

        MetricEntry entry;
        entry.snapshot.desc = _desc;
        entry.snapshot.integerValue = 0;
        entry.snapshot.doubleValue = 0.0;
        entry.snapshot.sampleCount = 0;
        entry.snapshot.sampleSum = 0.0;
        entry.snapshot.sampleMin = 0.0;
        entry.snapshot.sampleMax = 0.0;

#if defined(MENGINE_DOCUMENT_ENABLE)
        entry.doc = _doc;
#else
        MENGINE_UNUSED( _doc );
#endif

        m_metrics.emplace_back( entry );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetricService::unregisterMetric( const ConstString & _name, const ConstString & _owner )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        VectorMetrics::iterator it_found = StdAlgorithm::find_if( m_metrics.begin(), m_metrics.end(), [_name]( const MetricEntry & _entry )
        {
            return _entry.snapshot.desc.name == _name;
        } );

        if( it_found == m_metrics.end() || it_found->snapshot.desc.owner != _owner )
        {
            return false;
        }

        m_metrics.erase( it_found );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t MetricService::unregisterMetrics( const ConstString & _owner )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        VectorMetrics::size_type oldSize = m_metrics.size();

        m_metrics.erase( StdAlgorithm::remove_if( m_metrics.begin(), m_metrics.end(), [_owner]( const MetricEntry & _entry )
        {
            return _entry.snapshot.desc.owner == _owner;
        } ), m_metrics.end() );

        uint32_t count = (uint32_t)(oldSize - m_metrics.size());

        return count;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetricService::hasMetric( const ConstString & _name ) const
    {
        MENGINE_THREAD_SHARED_MUTEX_SCOPE( m_mutex );

        const MetricEntry * entry = this->findMetric_( _name );

        bool exist = entry != nullptr;

        return exist;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetricService::setMetricEnabled( const ConstString & _name, bool _enabled )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        MetricEntry * entry = this->findMetric_( _name );

        if( entry == nullptr )
        {
            return false;
        }

        entry->snapshot.desc.enabled = _enabled;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetricService::setMetricInteger( const ConstString & _name, int64_t _value )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        MetricEntry * entry = this->findMetric_( _name );

        if( entry == nullptr || entry->snapshot.desc.valueType != EMetricValueType::Integer || entry->snapshot.desc.type == EMetricType::Histogram )
        {
            return false;
        }

        entry->snapshot.integerValue = _value;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetricService::addMetricInteger( const ConstString & _name, int64_t _value )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        MetricEntry * entry = this->findMetric_( _name );

        if( entry == nullptr || entry->snapshot.desc.valueType != EMetricValueType::Integer || entry->snapshot.desc.type != EMetricType::Counter )
        {
            return false;
        }

        entry->snapshot.integerValue += _value;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetricService::setMetricDouble( const ConstString & _name, double _value )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        MetricEntry * entry = this->findMetric_( _name );

        if( entry == nullptr || entry->snapshot.desc.valueType != EMetricValueType::Double || entry->snapshot.desc.type == EMetricType::Histogram )
        {
            return false;
        }

        entry->snapshot.doubleValue = _value;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetricService::addMetricDouble( const ConstString & _name, double _value )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        MetricEntry * entry = this->findMetric_( _name );

        if( entry == nullptr || entry->snapshot.desc.valueType != EMetricValueType::Double || entry->snapshot.desc.type != EMetricType::Counter )
        {
            return false;
        }

        entry->snapshot.doubleValue += _value;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetricService::setMetricString( const ConstString & _name, const String & _value )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        MetricEntry * entry = this->findMetric_( _name );

        if( entry == nullptr || entry->snapshot.desc.valueType != EMetricValueType::String || entry->snapshot.desc.type != EMetricType::Gauge )
        {
            return false;
        }

        entry->snapshot.stringValue = _value;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetricService::recordMetricSample( const ConstString & _name, double _value )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        MetricEntry * entry = this->findMetric_( _name );

        if( entry == nullptr || entry->snapshot.desc.type != EMetricType::Histogram )
        {
            return false;
        }

        MetricSnapshot & snapshot = entry->snapshot;

        if( snapshot.sampleCount == 0 )
        {
            snapshot.sampleMin = _value;
            snapshot.sampleMax = _value;
        }
        else
        {
            snapshot.sampleMin = StdAlgorithm::min( snapshot.sampleMin, _value );
            snapshot.sampleMax = StdAlgorithm::max( snapshot.sampleMax, _value );
        }

        ++snapshot.sampleCount;
        snapshot.sampleSum += _value;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetricService::getMetric( const ConstString & _name, MetricSnapshot * const _snapshot ) const
    {
        if( _snapshot == nullptr )
        {
            return false;
        }

        MENGINE_THREAD_SHARED_MUTEX_SCOPE( m_mutex );

        const MetricEntry * entry = this->findMetric_( _name );

        if( entry == nullptr )
        {
            return false;
        }

        *_snapshot = entry->snapshot;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void MetricService::foreachMetrics( const LambdaMetric & _lambda ) const
    {
        Vector<MetricSnapshot> snapshots;

        {
            MENGINE_THREAD_SHARED_MUTEX_SCOPE( m_mutex );

            snapshots.reserve( m_metrics.size() );

            for( const MetricEntry & entry : m_metrics )
            {
                snapshots.emplace_back( entry.snapshot );
            }
        }

        for( const MetricSnapshot & snapshot : snapshots )
        {
            _lambda( snapshot );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    MetricService::MetricEntry * MetricService::findMetric_( const ConstString & _name )
    {
        VectorMetrics::iterator it_found = StdAlgorithm::find_if( m_metrics.begin(), m_metrics.end(), [_name]( const MetricEntry & _entry )
        {
            return _entry.snapshot.desc.name == _name;
        } );

        MetricEntry * entry = it_found != m_metrics.end() ? &*it_found : nullptr;

        return entry;
    }
    //////////////////////////////////////////////////////////////////////////
    const MetricService::MetricEntry * MetricService::findMetric_( const ConstString & _name ) const
    {
        VectorMetrics::const_iterator it_found = StdAlgorithm::find_if( m_metrics.begin(), m_metrics.end(), [_name]( const MetricEntry & _entry )
        {
            return _entry.snapshot.desc.name == _name;
        } );

        const MetricEntry * entry = it_found != m_metrics.end() ? &*it_found : nullptr;

        return entry;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MetricService::validateMetricDesc_( const MetricDesc & _desc ) const
    {
        if( _desc.name.empty() == true || _desc.owner.empty() == true )
        {
            LOGGER_ERROR( "metric name and owner must not be empty" );

            return false;
        }

        if( _desc.type == EMetricType::Histogram && _desc.valueType != EMetricValueType::Double )
        {
            LOGGER_ERROR( "histogram metric '%s' must use double values"
                , _desc.name.c_str()
            );

            return false;
        }

        if( _desc.valueType == EMetricValueType::String && _desc.type != EMetricType::Gauge )
        {
            LOGGER_ERROR( "string metric '%s' must be a gauge"
                , _desc.name.c_str()
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
