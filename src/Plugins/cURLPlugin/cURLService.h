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

    protected:
        HttpRequestID getMessage( const String & _url, int32_t _timeout, const cURLReceiverInterfacePtr & _receiver ) override;
        HttpRequestID postMessage( const String & _url, const MapParams & _params, int32_t _timeout, const cURLReceiverInterfacePtr & _receiver ) override;
        HttpRequestID headerData( const String & _url, const VectorString & _headers, const String & _data, int32_t _timeout, const cURLReceiverInterfacePtr & _receiver ) override;

    protected:
        HttpRequestID downloadAsset( const String & _url, const String & _login, const String & _password, const FileGroupInterfacePtr & _fileGroup, const FilePath & _path, int32_t _timeout, const cURLReceiverInterfacePtr & _receiver ) override;

    protected:
        bool cancelRequest( HttpRequestID _id ) override;

    private:
        void onHttpRequestComplete( HttpRequestID _id, uint32_t _status, const String & _error, const String & _response, uint32_t _code, bool _successful ) override;

    protected:
        ThreadQueueInterfacePtr m_threadQueue;

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