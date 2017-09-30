#	pragma once

#	include "Interface/ServiceInterface.h"

#	include "Core/String.h"
#	include "Core/ConstString.h"
#	include "Core/FilePath.h"
#	include "Core/Params.h"

#	include "stdex/stl_map.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	typedef uint32_t HttpRequestID;
	//////////////////////////////////////////////////////////////////////////
	class HttpReceiver
	{
	public:
		virtual void onHttpRequestComplete( HttpRequestID _id, uint32_t _status, const String & _response, uint32_t _code, bool _successful ) = 0;
	};
	//////////////////////////////////////////////////////////////////////////
    class HttpSystemInterface
		: public ServiceInterface
	{
		SERVICE_DECLARE("HttpSystem")

	public:
		virtual HttpRequestID getMessage( const String & _url, HttpReceiver * _receiver ) = 0;
		virtual HttpRequestID postMessage( const String & _url, const TMapParams & _params, HttpReceiver * _receiver ) = 0;		
        virtual HttpRequestID headerData( const String & _url, const TVectorString & _headers, const String & _data, HttpReceiver * _receiver ) = 0;

    public:
        virtual HttpRequestID downloadAsset( const String & _url, const String & _login, const String & _password, const ConstString & _category, const FilePath & _path, HttpReceiver * _receiver ) = 0;

	public:
		virtual bool cancelRequest( HttpRequestID _id ) = 0;
	};

#   define HTTP_SYSTEM( serviceProvider )\
	SERVICE_GET(serviceProvider, Menge::HttpSystemInterface)
}
