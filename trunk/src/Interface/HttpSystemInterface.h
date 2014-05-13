#	pragma once

#	include "Interface/ServiceInterface.h"

#	include "Core/String.h"
#	include "Core/ConstString.h"
#	include "Core/FilePath.h"

namespace Menge
{
	class HttpDownloadAssetReceiver
	{
	public:
		virtual void onDownloadAssetComplete( size_t _id, bool _successful ) = 0;
	};

    class HttpSystemInterface
		: public ServiceInterface
	{
		SERVICE_DECLARE("HttpSystem")

	public:
		virtual bool initialize() = 0;
		virtual void finalize() = 0;

	public:
		virtual size_t downloadAsset( const String & _url, const ConstString & _category, const FilePath & _path, HttpDownloadAssetReceiver * _receiver ) = 0;
		virtual bool cancelAsset( size_t _id ) = 0;
	};

#   define HTTP_SYSTEM( serviceProvider )\
	SERVICE_GET(serviceProvider, Menge::HttpSystemInterface)
}
