#pragma once

#include "Kernel/BaseHttpRequest.h"

#include "curl/curl.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class cURLHttpRequest
        : public BaseHttpRequest
    {
    public:
        cURLHttpRequest();
        ~cURLHttpRequest() override;

    protected:
        void _onThreadTaskPreparation() override;
        bool _onThreadTaskRun() override;
        bool _onThreadTaskProcess() override;

    protected:
        virtual void _onCURL( CURL * _curl ) = 0;
        virtual void _onCURLCleanup( CURL * _curl );

    protected:
        void setupWriteResponse( CURL * _curl );

    protected:
        void _onThreadTaskComplete( bool _successful ) override;

    public:
        void writeResponse( const Char * _ptr, size_t _size );
        void writeHeader( const Char * _ptr, size_t _size );

    protected:
        ThreadMutexInterfacePtr m_mutex;

    protected:
        struct curl_slist * m_curl_header_list;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<cURLHttpRequest, HttpRequestInterface> cURLHttpRequestPtr;
    //////////////////////////////////////////////////////////////////////////
}