#	pragma once

#	include "Interface/HttpSystemInterface.h"
#	include "Interface/StreamInterface.h"

#	include "Kernel/ThreadTask.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	class ThreadTaskGetAsset
		: public ThreadTask
	{
	public:
		ThreadTaskGetAsset();
		
	public:
		bool initialize( const String & _url, const String & _login, const String & _password, const ConstString & _category, const FilePath & _filepath, HttpRequestID _id, HttpReceiver * _receiver );

	protected:
		bool _onRun() override;

	protected:
		bool _onMain() override;

	protected:
		void _onComplete( bool _successful ) override;

	protected:		
		String m_url;
		String m_login;
		String m_password;
		ConstString m_category;
		FilePath m_filePath;

		HttpRequestID m_id;
		HttpReceiver * m_receiver;

		uint32_t m_code;
		OutputStreamInterfacePtr m_stream;

		bool m_successful;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<ThreadTaskGetAsset> ThreadTaskGetAssetPtr;
	//////////////////////////////////////////////////////////////////////////
}