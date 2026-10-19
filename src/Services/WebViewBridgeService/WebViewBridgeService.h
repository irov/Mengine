#pragma once

#include "Interface/WebViewBridgeServiceInterface.h"

#include "Kernel/Map.h"
#include "Kernel/ServiceBase.h"
#include "Kernel/Set.h"

namespace Mengine
{
    class WebViewBridgeService
        : public ServiceBase<WebViewBridgeServiceInterface>
    {
        DECLARE_FACTORABLE( WebViewBridgeService );

    public:
        WebViewBridgeService();
        ~WebViewBridgeService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        void setSender( const WebViewBridgeSenderInterfacePtr & _sender ) override;
        bool addHandler( const String & _name, const LambdaHandler & _handler ) override;
        void removeHandler( const String & _name ) override;
        uint64_t beginDocument() override;
        void closeDocument() override;
        uint64_t getDocumentGeneration() const override;
        bool isDocumentReady() const override;
        bool isAppReady() const override;
        bool receiveMessage( uint64_t _generation, const jpp::object & _message ) override;
        bool completeRequest( uint64_t _generation, const String & _id, const jpp::object & _result, const String & _error ) override;

    protected:
        typedef Map<String, LambdaHandler> MapHandlers;
        typedef Set<String> SetRequestIds;

    protected:
        MapHandlers m_handlers;
        SetRequestIds m_pending;
        WebViewBridgeSenderInterfacePtr m_sender;
        uint64_t m_generation;
        bool m_documentOpen;
        bool m_documentReady;
        bool m_appReady;
    };
}
