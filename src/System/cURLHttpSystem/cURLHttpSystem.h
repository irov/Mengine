#	pragma once

#	include "Interface/HttpSystemInterface.h"

#	include "Core/ServiceBase.h"

#	include "Factory/Factory.h"

#	include "Kernel/ThreadTask.h"

namespace Menge
{
	class cURLHttpSystem
		: public ServiceBase<HttpSystemInterface>
		, public HttpDownloadAssetReceiver
		, public HttpPostMessageReceiver
	{
	public:
		cURLHttpSystem();

	protected:
		bool _initialize() override;
		void _finalize() override;
		
	protected:
		HttpRequestID postMessage( const String & _url, const TMapParams & _params, HttpPostMessageReceiver * _receiver ) override;
		bool cancelPostMessage( HttpRequestID _id ) override;

	protected:
		HttpRequestID downloadAsset( const String & _url, const ConstString & _category, const FilePath & _path, HttpDownloadAssetReceiver * _receiver ) override;
		bool cancelDownloadAsset( HttpRequestID _id ) override;

	private:
		void onDownloadAssetComplete( HttpRequestID _id, bool _successful ) override;
		void onPostMessageComplete( HttpRequestID _id, const String & _response, uint32_t _code, bool _successful ) override;

	protected:
		uint32_t m_enumeratorDownloadAsset;

		struct DownloadAssetDesc
		{
			uint32_t id;
			ThreadTaskPtr task;
			HttpDownloadAssetReceiver * receiver;
		};

		typedef stdex::vector<DownloadAssetDesc> TVectorDownloadAssets;
		TVectorDownloadAssets m_downloadAssets;

		struct PostMessageDesc
		{
			uint32_t id;
			ThreadTaskPtr task;
			HttpPostMessageReceiver * receiver;
		};

		typedef stdex::vector<PostMessageDesc> TVectorPostMessages;
		TVectorPostMessages m_postMessages;

		FactoryPtr m_factoryTaskPostMessage;
		FactoryPtr m_factoryTaskDownloadAsset;
	};
}