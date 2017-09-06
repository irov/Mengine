#	pragma once

#	include "Interface/HttpSystemInterface.h"

#	include "Kernel/ThreadTask.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	class ThreadTaskHeaderData
		: public ThreadTask
	{
	public:
        ThreadTaskHeaderData();
		
	public:
		bool initialize( const String & _url, const TVectorString & _headers, const String & _data, HttpRequestID _id, HttpReceiver * _receiver );

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
        TVectorString m_headers;
        String m_data;

		HttpRequestID m_id;
		HttpReceiver * m_receiver;

		uint32_t m_code;
		String m_response;
		
		bool m_successful;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<ThreadTaskHeaderData> ThreadTaskHeaderDataPtr;
	//////////////////////////////////////////////////////////////////////////
}