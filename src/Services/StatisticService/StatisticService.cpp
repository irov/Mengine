#include "StatisticService.h"

#include "Kernel/ConfigHelper.h"
#include "Kernel/OptionHelper.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( StatisticService, Mengine::StatisticService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    StatisticService::StatisticService()
        : m_enable( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    StatisticService::~StatisticService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool StatisticService::_initializeService()
    {
        bool statistic = HAS_OPTION( "statistic" );

        if( statistic == true )
        {
            m_enable = true;
        }
        else
        {
            bool Statistic_Enable = CONFIG_VALUE( "Statistic", "Enable", MENGINE_DEBUG_VALUE( true, false ) );

            m_enable = Statistic_Enable;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void StatisticService::_finalizeService()
    {
        m_statisticIntegers.clear();
        m_statisticDoubles.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void StatisticService::addStatisticInteger( const ConstString & _name, int64_t _value )
    {
        if( m_enable == false )
        {
            return;
        }

        m_statisticIntegers[_name] += _value;
    }
    //////////////////////////////////////////////////////////////////////////
    bool StatisticService::hasStatisticInteger( const ConstString & _name ) const
    {
        if( m_enable == false )
        {
            return false;
        }

        if( m_statisticIntegers.find( _name ) == m_statisticIntegers.end() )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    int64_t StatisticService::getStatisticInteger( const ConstString & _name ) const
    {
        if( m_enable == false )
        {
            return 0;
        }

        MapStatisticIntegers::const_iterator it_found = m_statisticIntegers.find( _name );

        if( it_found == m_statisticIntegers.end() )
        {
            return 0;
        }

        int64_t value = it_found->second;

        return value;
    }
    //////////////////////////////////////////////////////////////////////////
    void StatisticService::resetStatisticInteger( const ConstString & _name )
    {
        if( m_enable == false )
        {
            return;
        }

        m_statisticIntegers[_name] = 0;
    }
    //////////////////////////////////////////////////////////////////////////
    void StatisticService::addStatisticDouble( const ConstString & _name, double _value )
    {
        if( m_enable == false )
        {
            return;
        }

        m_statisticDoubles[_name] += _value;
    }
    //////////////////////////////////////////////////////////////////////////
    bool StatisticService::hasStatisticDouble( const ConstString & _name ) const
    {
        if( m_enable == false )
        {
            return false;
        }

        if( m_statisticDoubles.find( _name ) == m_statisticDoubles.end() )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    double StatisticService::getStatisticDouble( const ConstString & _name ) const
    {
        if( m_enable == false )
        {
            return 0.0;
        }

        MapStatisticDoubles::const_iterator it_found = m_statisticDoubles.find( _name );

        if( it_found == m_statisticDoubles.end() )
        {
            return 0;
        }

        double value = it_found->second;

        return value;
    }
    //////////////////////////////////////////////////////////////////////////
    void StatisticService::resetStatisticDouble( const ConstString & _name )
    {
        if( m_enable == false )
        {
            return;
        }

        m_statisticDoubles[_name] = 0.0;
    }
    //////////////////////////////////////////////////////////////////////////
}
