#pragma once

#include "ThreadTaskCurl.h"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	class ThreadTaskGetMessage
		: public ThreadTaskCurl
	{
	public:
		ThreadTaskGetMessage();
        ~ThreadTaskGetMessage() override;
		
	public:
		bool initialize( const String & _url );

	protected:
		void _onCURL( CURL * _curl ) override;
        
	protected:		
		String m_url;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef IntrusivePtr<ThreadTaskGetMessage> ThreadTaskGetMessagePtr;
	//////////////////////////////////////////////////////////////////////////
}