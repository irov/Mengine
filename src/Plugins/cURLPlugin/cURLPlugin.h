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
        bool _initialize() override;
        void _finalize() override;

    protected:
        void _destroy() override;

    protected:
        HttpRequestID downloadAsset( const String & _url, const String & _login, const String & _password, const ConstString & _category, const FilePath & _filepath, const pybind::object & _cb, const pybind::args & _args );
        HttpRequestID postMessage( const String & _url, const MapParams & _params, const pybind::object & _cb, const pybind::args & _args );
        HttpRequestID headerData( const String & _url, const VectorString & _headers, const String & _data, const pybind::object & _cb, const pybind::args & _args );
        HttpRequestID getMessage( const String & _url, const pybind::object & _cb, const pybind::args & _args );
        void cancelRequest( HttpRequestID _id );

    protected:
        FactoryPtr m_factoryPyHttpReceiver;
    };
}

