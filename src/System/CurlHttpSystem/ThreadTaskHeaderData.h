#pragma once

#include "ThreadTaskCurl.h"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	class ThreadTaskHeaderData
		: public ThreadTaskCurl
	{
	public:
        ThreadTaskHeaderData();
		
	public:
		bool initialize( const String & _url, const TVectorString & _headers, const String & _data );

	protected:
        void _onCURL( CURL * _curl ) override;

	protected:		
		String m_url;
        TVectorString m_headers;
        String m_data;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<ThreadTaskHeaderData> ThreadTaskHeaderDataPtr;
	//////////////////////////////////////////////////////////////////////////
}