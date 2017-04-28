#	pragma once

#	include "Interface/ServiceInterface.h"

#	include "Core/String.h"
#	include "Core/ConstString.h"
#	include "Core/FilePath.h"
#	include "Core/Params.h"

#	include "stdex/stl_map.h"

namespace Menge
{
	typedef uint32_t HttpRequestID;

	class HttpDownloadAssetReceiver
	{
	public:
		virtual void onDownloadAssetComplete( HttpRequestID _id, bool _successful ) = 0;
	};

	class HttpPostMessageReceiver
	{
	public:
		virtual void onPostMessageComplete( HttpRequestID _id, const String & _response, uint32_t _code, bool _successful ) = 0;
	};

    class HttpSystemInterface
		: public ServiceInterface
	{
		SERVICE_DECLARE("HttpSystem")

	public:
		virtual HttpRequestID postMessage( const String & _url, const TMapParams & _params, HttpPostMessageReceiver * _receiver ) = 0;
		virtual bool cancelPostMessage( HttpRequestID _id ) = 0;

	public:
		virtual HttpRequestID downloadAsset( const String & _url, const ConstString & _category, const FilePath & _path, HttpDownloadAssetReceiver * _receiver ) = 0;
		virtual bool cancelDownloadAsset( HttpRequestID _id ) = 0;
	};

#   define HTTP_SYSTEM( serviceProvider )\
	SERVICE_GET(serviceProvider, Menge::HttpSystemInterface)
}
