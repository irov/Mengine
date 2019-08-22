#pragma once

#include "cURLInterface.h"

#include "Kernel/ThreadTask.h"

#include "curl/curl.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class cURLThreadTask
        : public ThreadTask
    {
    public:
        cURLThreadTask();
        ~cURLThreadTask() override;

    public:
        void setRequestId( HttpRequestID _id );
        HttpRequestID getRequestId() const;

        void setTimeout( int32_t _timeout );
        int32_t getTimeout() const;

        void setReceiver( const cURLReceiverInterfacePtr & _receiver );
        const cURLReceiverInterfacePtr & getReceiver() const;

    protected:
        bool _onRun() override;
        bool _onMain() override;

    protected:
        virtual void _onCURL( CURL * _curl ) = 0;
        virtual void _onCURLCleanup( CURL * _curl );

    protected:
        void setupWriteResponse( CURL * _curl );

    protected:
        void _onComplete( bool _successful ) override;

    public:
        void writeResponse( Char * _ptr, size_t _size );

    protected:
        HttpRequestID m_id;
        int32_t m_timeout;
        cURLReceiverInterfacePtr m_receiver;

        uint32_t m_code;
        CURLcode m_status;
        String m_error;
        String m_response;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<cURLThreadTask> cURLThreadTaskPtr;
    //////////////////////////////////////////////////////////////////////////
}