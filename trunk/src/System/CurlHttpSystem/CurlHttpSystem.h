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
		size_t downloadAsset( const String & _url, const ConstString & _category, const FilePath & _path, HttpDownloadAssetReceiver * _receiver ) override;
		bool cancelAsset( size_t _id ) override;

	private:
		void onDownloadAssetComplete( size_t _id, bool _successful ) override;

	protected:
		ServiceProviderInterface * m_serviceProvider;
		size_t m_enumeratorDownloadAsset;

		struct DownloadAssetDesc
		{
			size_t id;
			ThreadTaskPtr task;
			HttpDownloadAssetReceiver * receiver;
		};

		typedef stdex::vector<DownloadAssetDesc> TVectorDownloadAssets;
		TVectorDownloadAssets m_downloadAssets;
	};
}