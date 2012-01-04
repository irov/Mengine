#	pragma once

#	include "Interface/PluginInterface.h"
#	include "Interface/LogSystemInterface.h"
#	include "HTTPLogger.h"

namespace Menge
{
	/*
	class VideoCodecDecoderSystem
		: public DecoderSystemInterface
	{
	public:
		VideoCodecDecoderSystem( const String & _name );

	public:
		const String & getName() const;

	protected:
		void setService( CodecServiceInterface * _service ) override;

	protected:
		String m_name;
		CodecServiceInterface * m_service;
	};
	*/

	class HTTPLoggerPlugin
		: public PluginInterface
	{
	public:
		HTTPLoggerPlugin();

	protected:
		void initialize( ServiceProviderInterface * _provider, const TMapParam & _params ) override;
		void finalize() override;

	protected:
		typedef std::vector<HTTPLogger * > TVectorHTTPLoggers;
		TVectorHTTPLoggers m_loggers;
		LogServiceInterface * m_loggerService;
		
	};
}