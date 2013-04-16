#	include "HTTPLoggerPlugin.h"
#	include "HTTPLoggerCURL.h"
#	include "Utils/Core/File.h"
#	include "Interface/LogSystemInterface.h"
//////////////////////////////////////////////////////////////////////////
__declspec(dllexport) bool dllCreatePlugin( Menge::PluginInterface ** _plugin )
{
	*_plugin = new Menge::HTTPLoggerPlugin();
	return true;
}
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	HTTPLoggerPlugin::HTTPLoggerPlugin()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	bool HTTPLoggerPlugin::initialize( ServiceProviderInterface * _provider )
	{
		m_loggerService = _provider->getServiceT<LogServiceInterface>( "LogService" );

		if( m_loggerService == 0 )
		{
			return false;
		}
		
		HTTPServiceInterface * httpService = _provider->getServiceT<HTTPServiceInterface>( "HTTPService" );

		if( httpService == 0 )
		{
			return false;
		}

		HTTPSystemInterface * httpSystem = httpService->getInterface();		
		
		HTTPLoggerCURL * loggerCURL = new HTTPLoggerCURL( httpSystem );
		m_loggers.push_back( loggerCURL );
		
		for( TVectorHTTPLoggers::iterator
			it = m_loggers.begin(),
			it_end = m_loggers.end();
		it != it_end;
		++it )
		{
			m_loggerService->registerLogger( *(it) );
		}

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void HTTPLoggerPlugin::destroy()
	{
		for( TVectorHTTPLoggers::iterator
			it = m_loggers.begin(),
			it_end = m_loggers.end();
		it != it_end;
		++it )
		{
			m_loggerService->unregisterLogger( (*it) );

			delete (*it);
		}

		delete this;
	}
}