#	pragma once

#	include "ThreadTaskCurl.h"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	class ThreadTaskGetMessage
		: public ThreadTaskCurl
	{
	public:
		ThreadTaskGetMessage();
		
	public:
		bool initialize( const String & _url );

	protected:
		void _onCURL( CURL * _curl ) override;
        
	protected:		
		String m_url;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<ThreadTaskGetMessage> ThreadTaskGetMessagePtr;
	//////////////////////////////////////////////////////////////////////////
}