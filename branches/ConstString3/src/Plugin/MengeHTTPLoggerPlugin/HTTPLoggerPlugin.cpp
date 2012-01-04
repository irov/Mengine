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
	void HTTPLoggerPlugin::initialize( ServiceProviderInterface * _provider, const TMapParam & _params )
	{
		m_loggerService = _provider->getServiceT<LogServiceInterface>( "Log" );

		if( m_loggerService == 0 )
		{
			return;
		}
		
		HTTPServiceInterface * httpService = _provider->getServiceT<HTTPServiceInterface>( "HTTP" );

		if( httpService == 0 )
		{
			m_loggerService = 0;
			return;
		}

		HTTPSystemInterface * httpSystem = httpService->getInterface();
		LogSystemInterface * logSystem = m_loggerService->getInterface();
		
		HTTPLoggerCURL * loggerCURL = new HTTPLoggerCURL( httpSystem );
		m_loggers.push_back( loggerCURL );
		
		for( TVectorHTTPLoggers::iterator
			it = m_loggers.begin(),
			it_end = m_loggers.end();
		it != it_end;
		++it )
		{
			logSystem->registerLogger( *(it) );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void HTTPLoggerPlugin::finalize()
	{
		LogSystemInterface * logSystem = m_loggerService->getInterface();

		for( TVectorHTTPLoggers::iterator
			it = m_loggers.begin(),
			it_end = m_loggers.end();
		it != it_end;
		++it )
		{
			logSystem->unregisterLogger( (*it) );
			delete (*it);
		}
		delete this;
	}
}