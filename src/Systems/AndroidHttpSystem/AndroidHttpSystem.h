#pragma once

#include "Interface/FactoryInterface.h"
#include "Interface/HttpSystemInterface.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
    class AndroidHttpSystem
        : public ServiceBase<HttpSystemInterface>
    {
    public:
        AndroidHttpSystem();
        ~AndroidHttpSystem() override;

    protected:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        HttpRequestInterfacePtr createHttpRequestGetMessage( const DocumentInterfacePtr & _doc ) override;
        HttpRequestInterfacePtr createHttpRequestPostMessage( const HttpRequestPostProperties & _params, const DocumentInterfacePtr & _doc ) override;
        HttpRequestInterfacePtr createHttpRequestHeaderData( const Data & _data, const DocumentInterfacePtr & _doc ) override;
        HttpRequestInterfacePtr createHttpRequestDownloadAsset( const String & _login, const String & _password, const ContentInterfacePtr & _content, const DocumentInterfacePtr & _doc ) override;
        HttpRequestInterfacePtr createHttpRequestDeleteMessage( const DocumentInterfacePtr & _doc ) override;

    protected:
        FactoryInterfacePtr m_factoryTaskGetMessage;
        FactoryInterfacePtr m_factoryTaskPostMessage;
        FactoryInterfacePtr m_factoryTaskDeleteMessage;
        FactoryInterfacePtr m_factoryTaskHeaderData;
        FactoryInterfacePtr m_factoryTaskDownloadAsset;
    };
}