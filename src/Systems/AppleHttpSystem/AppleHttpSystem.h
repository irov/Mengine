#pragma once

#include "Interface/HttpSystemInterface.h"
#include "Interface/FactoryInterface.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
    class AppleHttpSystem
        : public ServiceBase<HttpSystemInterface>
    {
    public:
        AppleHttpSystem();
        ~AppleHttpSystem() override;

    protected:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        HttpRequestInterfacePtr createHttpRequestPing( const DocumentInterfacePtr & _doc ) override;

    public:
        HttpRequestInterfacePtr createHttpRequestGetMessage( const DocumentInterfacePtr & _doc ) override;
        HttpRequestInterfacePtr createHttpRequestPostMessage( const HttpRequestPostProperties & _params, const DocumentInterfacePtr & _doc ) override;
        HttpRequestInterfacePtr createHttpRequestHeaderData( const Data & _data, const DocumentInterfacePtr & _doc ) override;
        HttpRequestInterfacePtr createHttpRequestGetAsset( const String & _login, const String & _password, const ContentInterfacePtr & _content, const DocumentInterfacePtr & _doc ) override;
        HttpRequestInterfacePtr createHttpRequestDeleteMessage( const DocumentInterfacePtr & _doc ) override;

    protected:
        FactoryInterfacePtr m_factoryRequestPing;
        FactoryInterfacePtr m_factoryRequestGetMessage;
        FactoryInterfacePtr m_factoryRequestPostMessage;
        FactoryInterfacePtr m_factoryRequestDeleteMessage;
        FactoryInterfacePtr m_factoryRequestHeaderData;
        FactoryInterfacePtr m_factoryRequestGetAsset;
    };
}
