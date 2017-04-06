#	include "AEIOLogger.h"

#   include <iostream>

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	AEIOLogger::AEIOLogger(AEGP_SuiteHandler * _suites, AEGP_PluginID _aegp_plugin_id )
		: m_suites( _suites )
		, m_aegp_plugin_id( _aegp_plugin_id )
		, m_serviceProvider( nullptr )
		, m_verboseLevel( LM_INFO )
		, m_verboseFlag(0xFFFFFFFF)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	AEIOLogger::~AEIOLogger()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void AEIOLogger::setServiceProvider( ServiceProviderInterface * _serviceProvider )
	{
		m_serviceProvider = _serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	ServiceProviderInterface * AEIOLogger::getServiceProvider() const
	{
		return m_serviceProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	bool AEIOLogger::initialize()
	{ 
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void AEIOLogger::finalize()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void AEIOLogger::setVerboseLevel( EMessageLevel _level )
	{
		m_verboseLevel = _level;
	}
    //////////////////////////////////////////////////////////////////////////
    void AEIOLogger::setVerboseFlag( uint32_t _flag )
    {
        m_verboseFlag = _flag;
    }
	//////////////////////////////////////////////////////////////////////////
	bool AEIOLogger::validMessage( EMessageLevel _level, uint32_t _flag ) const
	{
		if( m_verboseLevel < _level )
		{
			return false;
		}

        if( _flag == 0 )
        {
            return true;
        }

        if( (m_verboseFlag & _flag) == 0 )
        {
            return false;
        }

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void AEIOLogger::log( EMessageLevel _level, uint32_t _flag, const char * _data, size_t _count )
	{	
		(void)_level;
        (void)_flag;
		(void)_count;
		
		m_suites->UtilitySuite5()->AEGP_ReportInfo( m_aegp_plugin_id, _data );
	}
	//////////////////////////////////////////////////////////////////////////
	void AEIOLogger::flush()
	{
		std::cout.flush();
	}
}	// namespace Menge
