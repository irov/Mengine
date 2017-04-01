#	pragma once

#	include "Interface/HttpSystemInterface.h"
#	include "Interface/StreamInterface.h"

#	include "Kernel/ThreadTask.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	class ThreadTaskDownloadAsset
		: public ThreadTask
	{
	public:
		ThreadTaskDownloadAsset();
		
	public:
		bool initialize( const String & _url, const ConstString & _category, const FilePath & _filepath, uint32_t _id, HttpDownloadAssetReceiver * _receiver );

	protected:
		bool _onRun() override;

	protected:
		bool _onMain() override;

	protected:
		void _onComplete( bool _successful ) override;

	protected:		
		String m_url;
		ConstString m_category;
		FilePath m_filePath;

		uint32_t m_id;
		HttpDownloadAssetReceiver * m_receiver;

		OutputStreamInterfacePtr m_stream;

		bool m_successful;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<ThreadTaskDownloadAsset> ThreadTaskDownloadAssetPtr;
	//////////////////////////////////////////////////////////////////////////
}