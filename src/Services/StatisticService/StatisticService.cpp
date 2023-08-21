#include "StatisticService.h"

#include "Interface/ThreadSystemInterface.h"

#include "Kernel/DocumentHelper.h"
#include "Kernel/ThreadSharedMutexScope.h"
#include "Kernel/AssertionMemoryPanic.h"

#include "Config/Algorithm.h"

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

        MENGINE_ASSERTION_MEMORY_PANIC( mutex );

        m_mutex = mutex;

        Algorithm::fill_n( m_statisticIntegers, MENGINE_STATISTIC_MAX_COUNT, 0LL );
        Algorithm::fill_n( m_statisticDoubles, MENGINE_STATISTIC_MAX_COUNT, 0.0 );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void StatisticService::_finalizeService()
    {
        m_mutex = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void StatisticService::addStatisticInteger( uint32_t _id, int64_t _value )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        m_statisticIntegers[_id] += _value;
    }
    //////////////////////////////////////////////////////////////////////////
    void StatisticService::delStatisticInteger( uint32_t _id, int64_t _value )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        m_statisticIntegers[_id] -= _value;
    }
    //////////////////////////////////////////////////////////////////////////
    int64_t StatisticService::getStatisticInteger( uint32_t _id ) const
    {
        MENGINE_THREAD_SHARED_MUTEX_SCOPE( m_mutex );

        int64_t value = m_statisticIntegers[_id];

        return value;
    }
    //////////////////////////////////////////////////////////////////////////
    void StatisticService::resetStatisticInteger( uint32_t _id )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        m_statisticIntegers[_id] = 0LL;
    }
    //////////////////////////////////////////////////////////////////////////
    void StatisticService::addStatisticDouble( uint32_t _id, double _value )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        m_statisticDoubles[_id] += _value;
    }
    //////////////////////////////////////////////////////////////////////////
    double StatisticService::getStatisticDouble( uint32_t _id ) const
    {
        MENGINE_THREAD_SHARED_MUTEX_SCOPE( m_mutex );

        double value = m_statisticDoubles[_id];

        return value;
    }
    //////////////////////////////////////////////////////////////////////////
    void StatisticService::resetStatisticDouble( uint32_t _id )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        m_statisticDoubles[_id] = 0.0;
    }
    //////////////////////////////////////////////////////////////////////////
    void StatisticService::setStatisticConstString( uint32_t _id, const ConstString & _value )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        m_statisticConstStrings[_id] = _value;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & StatisticService::getStatisticConstString( uint32_t _id ) const
    {
        MENGINE_THREAD_SHARED_MUTEX_SCOPE( m_mutex );

        const ConstString & value = m_statisticConstStrings[_id];

        return value;
    }
    //////////////////////////////////////////////////////////////////////////
    void StatisticService::resetStatisticConstString( uint32_t _id )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        m_statisticConstStrings[_id] = ConstString::none();
    }
    //////////////////////////////////////////////////////////////////////////
}
