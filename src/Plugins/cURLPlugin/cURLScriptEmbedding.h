#pragma once

#include "Interface/ScriptEmbeddingInterface.h"

#include "Environment/Python/PythonIncluder.h"

#include "cURLInterface.h"

#include "Kernel/Params.h"
#include "Kernel/Factorable.h"
#include "Kernel/Factory.h"

namespace Mengine
{
    class cURLScriptEmbedding
        : public ScriptEmbeddingInterface
        , public Factorable
    {
    public:
        cURLScriptEmbedding();
        ~cURLScriptEmbedding() override;

    public:
        bool embed( pybind::kernel_interface * _kernel ) override;
        void eject( pybind::kernel_interface * _kernel ) override;

    protected:
        HttpRequestID downloadAsset( const String & _url, const String & _login, const String & _password, const ConstString & _fileGroupName, const FilePath & _filepath, int32_t _timeout, const pybind::object & _cb, const pybind::args & _args );
        HttpRequestID postMessage( const String & _url, const MapParams & _params, int32_t _timeout, const pybind::object & _cb, const pybind::args & _args );
        HttpRequestID headerData( const String & _url, const VectorString & _headers, const String & _data, int32_t _timeout, const pybind::object & _cb, const pybind::args & _args );
        HttpRequestID getMessage( const String & _url, int32_t _timeout, const pybind::object & _cb, const pybind::args & _args );
        void cancelRequest( HttpRequestID _id );

    protected:
        FactoryPtr m_factoryPyHttpReceiver;
    };
}