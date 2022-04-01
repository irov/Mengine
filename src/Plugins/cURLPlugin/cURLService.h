#pragma once

#include "Interface/ThreadQueueInterface.h"

#include "cURLInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Factory.h"
#include "Kernel/ThreadTask.h"

namespace Mengine
{
    class cURLService
        : public ServiceBase<cURLServiceInterface>
        , public cURLReceiverInterface
    {
    public:
        cURLService();
        ~cURLService() override;

    protected:
        void _dependencyService() override;
        bool _initializeService() override;
        void _finalizeService() override;
        void _stopService() override;

    protected:
        HttpRequestID getMessage( const String & _url, const cURLHeaders & _headers, int32_t _timeout, bool _receiveHeaders, const cURLReceiverInterfacePtr & _receiver ) override;
        HttpRequestID postMessage( const String & _url, const cURLHeaders & _headers, int32_t _timeout, bool _receiveHeaders, const cURLPostParams & _params, const cURLReceiverInterfacePtr & _receiver ) override;
        HttpRequestID headerData( const String & _url, const cURLHeaders & _headers, int32_t _timeout, bool _receiveHeaders, const String & _data, const cURLReceiverInterfacePtr & _receiver ) override;

    protected:
        HttpRequestID downloadAsset( const String & _url, const String & _login, const String & _password, const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, int32_t _timeout, const cURLReceiverInterfacePtr & _receiver ) override;

    protected:
        bool cancelRequest( HttpRequestID _id ) override;

    protected:
        int32_t addRequestListener( const cURLRequestListenerInterfacePtr & _listener, const DocumentPtr & _doc ) override;
        void removeRequestListener( int32_t id ) override;

    protected:
        void onHttpRequestComplete( const cURLResponseData & _response ) override;

    protected:
        void notifyEnginePrepareFinalize_();

    protected:
        ThreadQueueInterfacePtr m_threadQueue;

        struct RequestListenerDesk
        {
            cURLRequestListenerInterfacePtr listener;
            int32_t id;

#if MENGINE_DOCUMENT_ENABLE
            DocumentPtr doc;
#endif
        };

        typedef Vector<RequestListenerDesk> VectorNetworkListeners;
        VectorNetworkListeners m_networkListeners;

        typedef Vector<ConstString> VectorThreads;
        VectorThreads m_threads;

        struct ReceiverDesc
        {
            uint32_t id;
            ThreadTaskPtr task;
            cURLReceiverInterfacePtr receiver;
        };

        typedef Vector<ReceiverDesc> VectorReceiverDesc;
        VectorReceiverDesc m_receiverDescs;
         
        FactoryPtr m_factoryTaskGetMessage;
        FactoryPtr m_factoryTaskPostMessage;
        FactoryPtr m_factoryTaskHeaderData;
        FactoryPtr m_factoryTaskDownloadAsset;
    };
}