#pragma once

#include "Interface/ScriptEmbeddingInterface.h"
#include "Interface/FactoryInterface.h"
#include "Interface/HttpSystemInterface.h"

#include "Environment/Python/PythonIncluder.h"

#include "Kernel/Params.h"
#include "Kernel/Factorable.h"

namespace Mengine
{
    class HttpSystemScriptEmbedding
        : public ScriptEmbeddingInterface
        , public Factorable
    {
    public:
        HttpSystemScriptEmbedding();
        ~HttpSystemScriptEmbedding() override;

    public:
        bool embed( pybind::kernel_interface * _kernel ) override;
        void eject( pybind::kernel_interface * _kernel ) override;

    protected:
        HttpRequestId downloadAsset( const String & _url, const String & _login, const String & _password, const ConstString & _fileGroupName, const FilePath & _filepath, int32_t _timeout, const pybind::object & _cb, const pybind::args & _args );
        HttpRequestId postMessage( const String & _url, const MapParams & _params, int32_t _timeout, const pybind::object & _cb, const pybind::args & _args );
        HttpRequestId headerData( const String & _url, const VectorString & _headers, const Data & _data, int32_t _timeout, const pybind::object & _cb, const pybind::args & _args );
        HttpRequestId getMessage( const String & _url, int32_t _timeout, const pybind::object & _cb, const pybind::args & _args );
        void cancelRequest( HttpRequestId _id );

    protected:
        FactoryInterfacePtr m_factoryPyHttpReceiver;
    };
}