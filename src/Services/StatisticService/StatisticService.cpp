#include "StatisticService.h"

#include "Interface/ThreadSystemInterface.h"

#include "Kernel/ConfigHelper.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/ThreadMutexScope.h"

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
            ThreadSystemInterface::getStaticServiceID()
        };

        return required;
    }
    //////////////////////////////////////////////////////////////////////////
    bool StatisticService::_availableService() const
    {
        if( HAS_OPTION( "statistic" ) == true )
        {
            return true;
        }

        if( HAS_OPTION( "nostatistic" ) == true )
        {
            return false;
        }

        bool Statistic_Enable = CONFIG_VALUE( "Statistic", "Available", MENGINE_DEBUG_VALUE( true, false ) );

        if( Statistic_Enable == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool StatisticService::_initializeService()
    {
        m_mutex = THREAD_SYSTEM()
            ->createMutex( MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void StatisticService::_finalizeService()
    {
        m_statisticIntegers.clear();
        m_statisticDoubles.clear();
        m_statisticConstStrings.clear();

        m_mutex = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void StatisticService::addStatisticInteger( const ConstString & _name, int64_t _value )
    {
        if( this->isAvailableService() == false )
        {
            return;
        }

        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        m_statisticIntegers[_name] += _value;
    }
    //////////////////////////////////////////////////////////////////////////
    bool StatisticService::hasStatisticInteger( const ConstString & _name ) const
    {
        if( this->isAvailableService() == false )
        {
            return false;
        }

        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        MapStatisticIntegers::const_iterator it_found = m_statisticIntegers.find( _name );

        if( it_found == m_statisticIntegers.end() )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    int64_t StatisticService::getStatisticInteger( const ConstString & _name ) const
    {
        if( this->isAvailableService() == false )
        {
            return 0LL;
        }

        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        MapStatisticIntegers::const_iterator it_found = m_statisticIntegers.find( _name );

        if( it_found == m_statisticIntegers.end() )
        {
            return 0LL;
        }

        int64_t value = it_found->second;

        return value;
    }
    //////////////////////////////////////////////////////////////////////////
    void StatisticService::resetStatisticInteger( const ConstString & _name )
    {
        if( this->isAvailableService() == false )
        {
            return;
        }

        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        m_statisticIntegers.erase( _name );
    }
    //////////////////////////////////////////////////////////////////////////
    void StatisticService::addStatisticDouble( const ConstString & _name, double _value )
    {
        if( this->isAvailableService() == false )
        {
            return;
        }

        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        m_statisticDoubles[_name] += _value;
    }
    //////////////////////////////////////////////////////////////////////////
    bool StatisticService::hasStatisticDouble( const ConstString & _name ) const
    {
        if( this->isAvailableService() == false )
        {
            return false;
        }

        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        MapStatisticDoubles::const_iterator it_found = m_statisticDoubles.find( _name );

        if( it_found == m_statisticDoubles.end() )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    double StatisticService::getStatisticDouble( const ConstString & _name ) const
    {
        if( this->isAvailableService() == false )
        {
            return 0.0;
        }

        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        MapStatisticDoubles::const_iterator it_found = m_statisticDoubles.find( _name );

        if( it_found == m_statisticDoubles.end() )
        {
            return 0.0;
        }

        double value = it_found->second;

        return value;
    }
    //////////////////////////////////////////////////////////////////////////
    void StatisticService::resetStatisticDouble( const ConstString & _name )
    {
        if( this->isAvailableService() == false )
        {
            return;
        }

        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        m_statisticDoubles.erase( _name );
    }
    //////////////////////////////////////////////////////////////////////////
    void StatisticService::setStatisticConstString( const ConstString & _name, const ConstString & _value )
    {
        if( this->isAvailableService() == false )
        {
            return;
        }

        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        m_statisticConstStrings[_name] = _value;
    }
    //////////////////////////////////////////////////////////////////////////
    bool StatisticService::hasStatisticConstString( const ConstString & _name ) const
    {
        if( this->isAvailableService() == false )
        {
            return false;
        }

        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        MapStatisticConstStrings::const_iterator it_found = m_statisticConstStrings.find( _name );

        if( it_found == m_statisticConstStrings.end() )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & StatisticService::getStatisticConstString( const ConstString & _name ) const
    {
        if( this->isAvailableService() == false )
        {
            return ConstString::none();
        }

        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        MapStatisticConstStrings::const_iterator it_found = m_statisticConstStrings.find( _name );

        if( it_found == m_statisticConstStrings.end() )
        {
            return ConstString::none();
        }

        const ConstString & value = it_found->second;

        return value;
    }
    //////////////////////////////////////////////////////////////////////////
    void StatisticService::resetStatisticConstString( const ConstString & _name )
    {
        if( this->isAvailableService() == false )
        {
            return;
        }

        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        m_statisticConstStrings.erase( _name );
    }
    //////////////////////////////////////////////////////////////////////////
}
