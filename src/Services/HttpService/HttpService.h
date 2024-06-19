#pragma once

#include "Interface/ThreadQueueInterface.h"
#include "Interface/FactoryInterface.h"
#include "Interface/ThreadTaskInterface.h"
#include "Interface/HttpServiceInterface.h"

#include "Kernel/ServiceBase.h"

#include "Config/Timestamp.h"

namespace Mengine
{
    class HttpService
        : public ServiceBase<HttpServiceInterface>
        , public HttpReceiverInterface
    {
    public:
        HttpService();
        ~HttpService() override;

    protected:
        const ServiceRequiredList & requiredServices() const override;
        bool _availableService() const override;
        bool _initializeService() override;
        void _finalizeService() override;
        void _stopService() override;

    protected:
        void setProxy( const String & _proxy ) override;
        const String & getProxy() const override;

    protected:
        const HttpRequestHeaders & getApplicationJSONHeaders() const override;

    protected:
        HttpRequestId getMessage( const String & _url, const HttpRequestHeaders & _headers, int32_t _timeout, bool _receiveHeaders, const HttpReceiverInterfacePtr & _receiver, const DocumentInterfacePtr & _doc ) override;
        HttpRequestId postMessage( const String & _url, const HttpRequestHeaders & _headers, int32_t _timeout, bool _receiveHeaders, const HttpRequestPostProperties & _params, const HttpReceiverInterfacePtr & _receiver, const DocumentInterfacePtr & _doc ) override;
        HttpRequestId deleteMessage( const String & _url, const HttpRequestHeaders & _headers, int32_t _timeout, bool _receiveHeaders, const HttpReceiverInterfacePtr & _receiver, const DocumentInterfacePtr & _doc ) override;
        HttpRequestId headerData( const String & _url, const HttpRequestHeaders & _headers, int32_t _timeout, bool _receiveHeaders, const Data & _data, const HttpReceiverInterfacePtr & _receiver, const DocumentInterfacePtr & _doc ) override;
        HttpRequestId getAsset( const String & _url, const String & _login, const String & _password, const ContentInterfacePtr & _content, int32_t _timeout, const HttpReceiverInterfacePtr & _receiver, const DocumentInterfacePtr & _doc ) override;

    protected:
        bool cancelRequest( HttpRequestId _id ) override;

    protected:
        void onHttpRequestComplete( const HttpResponseInterfacePtr & _response ) override;

    protected:
        void notifyEnginePrepareFinalize_();

    protected:
        ThreadQueueInterfacePtr m_threadQueue;

        ThreadMutexInterfacePtr m_mutex;

        String m_proxy;

        typedef Vector<ConstString> VectorThreads;
        VectorThreads m_threads;

        enum EReceiverType
        {
            ERT_GET_MESSAGE,
            ERT_POST_MESSAGE,
            ERT_HEADER_DATA,
            ERT_DOWNLOAD_ASSET
        };

        struct ReceiverDesc
        {
            HttpRequestId id;
            EReceiverType type;
            Timestamp timestamp;
            ThreadTaskInterfacePtr task;
            HttpReceiverInterfacePtr receiver;

#if defined(MENGINE_DOCUMENT_ENABLE)
            DocumentInterfacePtr doc;
#endif
        };

        typedef Vector<ReceiverDesc> VectorReceiverDesc;
        VectorReceiverDesc m_receiverDescs;

        HttpRequestHeaders m_applicationJSONHeaders;

        FactoryInterfacePtr m_factoryResponse;
    };
}