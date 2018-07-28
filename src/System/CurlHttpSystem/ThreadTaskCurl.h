#pragma once

#include "Interface/HttpSystemInterface.h"

#include "Kernel/ThreadTask.h"

#include "curl/curl.h"

namespace Mengine
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

        void setReceiver( const HttpReceiverInterfacePtr & _receiver );
        const HttpReceiverInterfacePtr & getReceiver() const;

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
        HttpReceiverInterfacePtr m_receiver;

		uint32_t m_code;
        CURLcode m_status;
		String m_response;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef IntrusivePtr<ThreadTaskCurl> ThreadTaskCurlPtr;
	//////////////////////////////////////////////////////////////////////////
}