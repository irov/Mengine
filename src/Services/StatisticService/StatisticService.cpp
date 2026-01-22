#include "StatisticService.h"

#include "Interface/ThreadSystemInterface.h"

#include "Kernel/ThreadSharedMutexScope.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/StatisticDetail.h"

#include "Config/StdAlgorithm.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( StatisticService, Mengine::StatisticService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    StatisticService::StatisticService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    StatisticService::~StatisticService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    const ServiceRequiredList & StatisticService::requiredServices() const
    {
        static ServiceRequiredList required = {
            SERVICE_ID( ThreadSystemInterface )
        };

        return required;
    }
    //////////////////////////////////////////////////////////////////////////
    bool StatisticService::_initializeService()
    {
        ThreadSharedMutexInterfacePtr mutex = THREAD_SYSTEM()
            ->createSharedMutex( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( mutex, "invalid create shared mutex" );

        m_mutex = mutex;

        for( uint32_t index = 0; index != MENGINE_STATISTIC_MAX_COUNT; ++index )
        {
            m_statisticEnabled[index] = Helper::isStatisticDefaultEnabled( (StatisticId)index );
        }

        StdAlgorithm::fill_n( m_statisticIntegers, MENGINE_STATISTIC_MAX_COUNT, MENGINE_INT64_C( 0 ) );
        StdAlgorithm::fill_n( m_statisticDoubles, MENGINE_STATISTIC_MAX_COUNT, 0.0 );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void StatisticService::_finalizeService()
    {
        m_mutex = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void StatisticService::setStatisticEnabled( StatisticId _id, bool _enable )
    {
        m_statisticEnabled[_id] = _enable;
    }
    //////////////////////////////////////////////////////////////////////////
    bool StatisticService::isStatisticEnabled( StatisticId _id ) const
    {
        bool value = m_statisticEnabled[_id];

        return value;
    }
    //////////////////////////////////////////////////////////////////////////
    void StatisticService::addStatisticInteger( StatisticId _id, int64_t _value )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        m_statisticIntegers[_id] += _value;
    }
    //////////////////////////////////////////////////////////////////////////
    void StatisticService::delStatisticInteger( StatisticId _id, int64_t _value )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        m_statisticIntegers[_id] -= _value;
    }
    //////////////////////////////////////////////////////////////////////////
    int64_t StatisticService::getStatisticInteger( StatisticId _id ) const
    {
        MENGINE_THREAD_SHARED_MUTEX_SCOPE( m_mutex );

        int64_t value = m_statisticIntegers[_id];

        return value;
    }
    //////////////////////////////////////////////////////////////////////////
    void StatisticService::resetStatisticInteger( StatisticId _id )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        m_statisticIntegers[_id] = MENGINE_INT64_C( 0 );
    }
    //////////////////////////////////////////////////////////////////////////
    void StatisticService::addStatisticDouble( StatisticId _id, double _value )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        m_statisticDoubles[_id] += _value;
    }
    //////////////////////////////////////////////////////////////////////////
    double StatisticService::getStatisticDouble( StatisticId _id ) const
    {
        MENGINE_THREAD_SHARED_MUTEX_SCOPE( m_mutex );

        double value = m_statisticDoubles[_id];

        return value;
    }
    //////////////////////////////////////////////////////////////////////////
    void StatisticService::resetStatisticDouble( StatisticId _id )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        m_statisticDoubles[_id] = 0.0;
    }
    //////////////////////////////////////////////////////////////////////////
    void StatisticService::setStatisticConstString( StatisticId _id, const ConstString & _value )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        m_statisticConstStrings[_id] = _value;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & StatisticService::getStatisticConstString( StatisticId _id ) const
    {
        MENGINE_THREAD_SHARED_MUTEX_SCOPE( m_mutex );

        const ConstString & value = m_statisticConstStrings[_id];

        return value;
    }
    //////////////////////////////////////////////////////////////////////////
    void StatisticService::resetStatisticConstString( StatisticId _id )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        m_statisticConstStrings[_id] = ConstString::none();
    }
    //////////////////////////////////////////////////////////////////////////
}
