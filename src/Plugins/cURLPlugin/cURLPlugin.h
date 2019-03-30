#pragma once

#include "cURLInterface.h"

#include "Kernel/PluginBase.h"
#include "Kernel/Factory.h"

#include "pybind/pybind.hpp"

namespace Mengine
{
    class cURLPlugin
        : public PluginBase
    {
    public:
        PLUGIN_DECLARE( "cURL" );

    public:
        cURLPlugin();
        ~cURLPlugin() override;

    protected:
        bool _initializePlugin() override;
        void _finalizePlugin() override;

    protected:
        void _destroy() override;

    protected:
        HttpRequestID downloadAsset( const String & _url, const String & _login, const String & _password, const ConstString & _category, const FilePath & _filepath, int32_t _timeout, const pybind::object & _cb, const pybind::args & _args );
        HttpRequestID postMessage( const String & _url, const MapParams & _params, int32_t _timeout, const pybind::object & _cb, const pybind::args & _args );
        HttpRequestID headerData( const String & _url, const VectorString & _headers, const String & _data, int32_t _timeout, const pybind::object & _cb, const pybind::args & _args );
        HttpRequestID getMessage( const String & _url, int32_t _timeout, const pybind::object & _cb, const pybind::args & _args );
        void cancelRequest( HttpRequestID _id );

    protected:
        FactoryPtr m_factoryPyHttpReceiver;
    };
}

