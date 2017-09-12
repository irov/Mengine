#	pragma once

#	include "Interface/StreamInterface.h"

#	include "ThreadTaskCurl.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	class ThreadTaskGetAsset
		: public ThreadTaskCurl
	{
	public:
		ThreadTaskGetAsset();
		
	public:
		bool initialize( const String & _url, const String & _login, const String & _password, const ConstString & _category, const FilePath & _filepath );

	protected:
        bool _onRun() override;

    protected:
		void _onCURL( CURL * _curl ) override;

    protected:
        void _onComplete( bool _successful ) override;

	protected:		
		String m_url;
		String m_login;
		String m_password;
		ConstString m_category;
		FilePath m_filePath;

		OutputStreamInterfacePtr m_stream;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<ThreadTaskGetAsset> ThreadTaskGetAssetPtr;
	//////////////////////////////////////////////////////////////////////////
}