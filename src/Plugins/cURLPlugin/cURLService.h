#pragma once

#include "Interface/ThreadQueueInterface.h"
#include "Interface/FactoryInterface.h"
#include "Interface/ThreadTaskInterface.h"

#include "cURLInterface.h"

#include "Kernel/ServiceBase.h"

#include "Config/Time.h"

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
        HttpRequestID getMessage( const String & _url, const cURLHeaders & _headers, int32_t _timeout, bool _receiveHeaders, const cURLReceiverInterfacePtr & _receiver, const DocumentPtr & _doc ) override;
        HttpRequestID postMessage( const String & _url, const cURLHeaders & _headers, int32_t _timeout, bool _receiveHeaders, const cURLPostParams & _params, const cURLReceiverInterfacePtr & _receiver, const DocumentPtr & _doc ) override;
        HttpRequestID headerData( const String & _url, const cURLHeaders & _headers, int32_t _timeout, bool _receiveHeaders, const String & _data, const cURLReceiverInterfacePtr & _receiver, const DocumentPtr & _doc ) override;

    protected:
        HttpRequestID downloadAsset( const String & _url, const String & _login, const String & _password, const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, int32_t _timeout, const cURLReceiverInterfacePtr & _receiver, const DocumentPtr & _doc ) override;

    protected:
        bool cancelRequest( HttpRequestID _id ) override;

    protected:
        int32_t addRequestListener( const cURLRequestListenerInterfacePtr & _listener, const DocumentPtr & _doc ) override;
        void removeRequestListener( int32_t id ) override;

    protected:
        void onHttpRequestComplete( const cURLResponseInterfacePtr & _response ) override;

    protected:
        void notifyEnginePrepareFinalize_();

    protected:
        ThreadQueueInterfacePtr m_threadQueue;

        ThreadMutexInterfacePtr m_mutex;

        struct RequestListenerDesk
        {
            cURLRequestListenerInterfacePtr listener;
            int32_t id;

#ifdef MENGINE_DOCUMENT_ENABLE
            DocumentPtr doc;
#endif
        };

        typedef Vector<RequestListenerDesk> VectorNetworkListeners;
        VectorNetworkListeners m_networkListeners;

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
            uint32_t id;
            EReceiverType type;
            TimeMilliseconds timestamp;
            ThreadTaskInterfacePtr task;
            cURLReceiverInterfacePtr receiver;

#ifdef MENGINE_DOCUMENT_ENABLE
            DocumentPtr doc;
#endif
        };

        typedef Vector<ReceiverDesc> VectorReceiverDesc;
        VectorReceiverDesc m_receiverDescs;
        
        FactoryInterfacePtr m_factoryResponse;
        FactoryInterfacePtr m_factoryTaskGetMessage;
        FactoryInterfacePtr m_factoryTaskPostMessage;
        FactoryInterfacePtr m_factoryTaskHeaderData;
        FactoryInterfacePtr m_factoryTaskDownloadAsset;
    };
}