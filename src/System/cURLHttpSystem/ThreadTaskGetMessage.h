#	pragma once

#	include "Interface/HttpSystemInterface.h"

#	include "Kernel/ThreadTask.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	class ThreadTaskGetMessage
		: public ThreadTask
	{
	public:
		ThreadTaskGetMessage();
		
	public:
		bool initialize( const String & _url, HttpRequestID _id, HttpReceiver * _receiver );

	protected:
		bool _onRun() override;

	protected:
		bool _onMain() override;

	protected:
		void _onComplete( bool _successful ) override;

	public:
		void writeResponse( char * _ptr, size_t _size );

	protected:		
		String m_url;

		HttpRequestID m_id;
		HttpReceiver * m_receiver;

		uint32_t m_code;
		String m_response;
		
		bool m_successful;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<ThreadTaskGetMessage> ThreadTaskGetMessagePtr;
	//////////////////////////////////////////////////////////////////////////
}