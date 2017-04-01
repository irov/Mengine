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
	{
	public:
		cURLHttpSystem();

	protected:
		bool _initialize() override;
		void _finalize() override;
		
	protected:
		HttpAssetID downloadAsset( const String & _url, const ConstString & _category, const FilePath & _path, HttpDownloadAssetReceiver * _receiver ) override;
		bool cancelAsset( HttpAssetID _id ) override;

	private:
		void onDownloadAssetComplete( HttpAssetID _id, bool _successful ) override;

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

		FactoryPtr m_factoryTaskDownloadAsset;
	};
}