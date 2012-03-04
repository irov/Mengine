#	pragma once

#	include <curl/curl.h>
#	include <curl/types.h>
#	include <curl/easy.h>

#	include "CURLHTTPRequestPerformer.h"
#	include "Interface/PluginInterface.h"
#	include "Interface/HTTPSystemInterface.h"
#	include "Interface/LogSystemInterface.h"

#	define CURL_HTTP_SYSTEM_ERROR_BUFFER_SIZE 255

namespace Menge
{

    class CURLHTTPSystem 
		: public HTTPSystemInterface
		, public HTTPServiceInterface
		, public PluginInterface
	{
	public:
		CURLHTTPSystem();
		virtual ~CURLHTTPSystem();
	
	public:
		void initialize( ServiceProviderInterface * _provider, const TMapParam & _params ) override;
		void finalize() override;

	public:
		HTTPSystemInterface* getInterface() override;	

	public:
		void send( const HTTPRequest& _request, HTTPResponseReceiver * _receiver ) override;
	    void sendAsync( const HTTPRequest& _request, HTTPResponseReceiver * _receiver ) override;
		void update() override;

	protected:
		ServiceProviderInterface * m_provider;
        char m_errorBuffer  [ CURL_HTTP_SYSTEM_ERROR_BUFFER_SIZE ];
		LogServiceInterface * m_logService;
		typedef std::vector<CURLHTTPRequestPerformer*> THTTPRequestPerformersVector;
		THTTPRequestPerformersVector m_performers;
	};

}