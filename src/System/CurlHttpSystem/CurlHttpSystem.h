#	pragma once

#	include "Interface/HttpSystemInterface.h"

#	include "Kernel/ThreadTask.h"

namespace Menge
{
	class CurlHttpSystem
		: public HttpSystemInterface
		, public HttpDownloadAssetReceiver
	{
	public:
		CurlHttpSystem();

	protected:
		void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
		ServiceProviderInterface * getServiceProvider() const override;

	protected:
		bool initialize() override;
		void finalize() override;
		
	protected:
		uint32_t downloadAsset( const String & _url, const ConstString & _category, const FilePath & _path, HttpDownloadAssetReceiver * _receiver ) override;
		bool cancelAsset( uint32_t _id ) override;

	private:
		void onDownloadAssetComplete( uint32_t _id, bool _successful ) override;

	protected:
		ServiceProviderInterface * m_serviceProvider;
		uint32_t m_enumeratorDownloadAsset;

		struct DownloadAssetDesc
		{
			uint32_t id;
			ThreadTaskPtr task;
			HttpDownloadAssetReceiver * receiver;
		};

		typedef stdex::vector<DownloadAssetDesc> TVectorDownloadAssets;
		TVectorDownloadAssets m_downloadAssets;
	};
}