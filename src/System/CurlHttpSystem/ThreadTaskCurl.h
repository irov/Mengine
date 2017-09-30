#	pragma once

#	include "Interface/HttpSystemInterface.h"

#	include "Kernel/ThreadTask.h"

#	include "curl/curl.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	class ThreadTaskCurl
		: public ThreadTask
	{
	public:
        ThreadTaskCurl();
		
    public:
        void setRequestId( HttpRequestID _id );
        HttpRequestID getRequestId() const;

        void setReceiver( HttpReceiver * _receiver );
        HttpReceiver * getReceiver() const;

	protected:
		bool _onRun() override;
        bool _onMain() override;

    protected:
        virtual void _onCURL( CURL * _curl ) = 0;

    protected:
        void setupWriteResponse( CURL * _curl );

	protected:
		void _onComplete( bool _successful ) override;

	public:        
		void writeResponse( char * _ptr, size_t _size );

	protected:		
		HttpRequestID m_id;
		HttpReceiver * m_receiver;

		uint32_t m_code;
        CURLcode m_status;
		String m_response;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<ThreadTaskCurl> ThreadTaskCurlPtr;
	//////////////////////////////////////////////////////////////////////////
}