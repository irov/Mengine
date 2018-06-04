#pragma once

#include "ThreadTaskCurl.h"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	class ThreadTaskPostMessage
		: public ThreadTaskCurl
	{
	public:
		ThreadTaskPostMessage();
		
	public:
		bool initialize( const String & _url, const TMapParams & _params );

	protected:
		void _onCURL( CURL * _curl ) override;

	protected:		
		String m_url;
        TMapParams m_params;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<ThreadTaskPostMessage> ThreadTaskPostMessagePtr;
	//////////////////////////////////////////////////////////////////////////
}