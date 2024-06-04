#pragma once

#include "Interface/HttpSystemInterface.h"
#include "Interface/FactoryInterface.h"

#include "Environment/Windows/WindowsIncluder.h"

#include "Kernel/ServiceBase.h"

namespace Mengine
{
    class Win32HttpSystem
        : public ServiceBase<HttpSystemInterface>
    {
    public:
        Win32HttpSystem();
        ~Win32HttpSystem() override;

    protected:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        HttpRequestInterfacePtr createHttpRequestGetMessage( const DocumentInterfacePtr & _doc ) override;
        HttpRequestInterfacePtr createHttpRequestPostMessage( const HttpRequestPostProperties & _params, const DocumentInterfacePtr & _doc ) override;
        HttpRequestInterfacePtr createHttpRequestHeaderData( const Data & _data, const DocumentInterfacePtr & _doc ) override;
        HttpRequestInterfacePtr createHttpRequestGetAsset( const String & _login, const String & _password, const ContentInterfacePtr & _content, const DocumentInterfacePtr & _doc ) override;
        HttpRequestInterfacePtr createHttpRequestDeleteMessage( const DocumentInterfacePtr & _doc ) override;

    protected:
        HINTERNET m_hInternet;

        FactoryInterfacePtr m_factoryTaskGetMessage;
        FactoryInterfacePtr m_factoryTaskPostMessage;
        FactoryInterfacePtr m_factoryTaskDeleteMessage;
        FactoryInterfacePtr m_factoryTaskHeaderData;
        FactoryInterfacePtr m_factoryTaskGetAsset;
    };
}
