#	pragma once

#	include "Core/PluginBase.h"

#	include <curl/curl.h>
#	include <curl/types.h>
#	include <curl/easy.h>

#	include "cURLRequestPerformer.h"

#	include "Interface/HTTPSystemInterface.h"

#	define CURL_HTTP_SYSTEM_ERROR_BUFFER_SIZE 255

namespace Menge
{

    class cURLPlugin
		, public PluginBase
	{
	public:
		cURLPlugin();
		~cURLPlugin();
	

    public:
        bool _initialize() override;
		void _finalize() override;

	public:
		void send( const HTTPRequest& _request, HTTPResponseReceiver * _receiver ) override;
	    void sendAsync( const HTTPRequest& _request, HTTPResponseReceiver * _receiver ) override;
		void update() override;

	protected:
		ServiceProviderInterface * m_serviceProvider;

        char m_errorBuffer[CURL_HTTP_SYSTEM_ERROR_BUFFER_SIZE];
		typedef std::vector<CURLHTTPRequestPerformer*> THTTPRequestPerformersVector;
		THTTPRequestPerformersVector m_performers;
	};
}