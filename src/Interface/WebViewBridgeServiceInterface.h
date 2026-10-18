#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/WebViewBridgeSenderInterface.h"

#include "Kernel/JSON.h"
#include "Kernel/String.h"

#include "Config/Lambda.h"

namespace Mengine
{
    class WebViewBridgeServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "WebViewBridgeService" );

    public:
        typedef Lambda<void( uint64_t, const String &, const jpp::object & )> LambdaHandler;

    public:
        virtual void setSender( const WebViewBridgeSenderInterfacePtr & _sender ) = 0;
        virtual bool addHandler( const String & _name, const LambdaHandler & _handler ) = 0;
        virtual void removeHandler( const String & _name ) = 0;
        virtual uint64_t beginDocument() = 0;
        virtual void closeDocument() = 0;
        virtual uint64_t getDocumentGeneration() const = 0;
        virtual bool isDocumentReady() const = 0;
        virtual bool isAppReady() const = 0;
        virtual bool receiveMessage( uint64_t _generation, const jpp::object & _message ) = 0;
        virtual bool completeRequest( uint64_t _generation, const String & _id, const jpp::object & _result, const String & _error ) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define WEBVIEW_BRIDGE_SERVICE()\
    ((Mengine::WebViewBridgeServiceInterface *)SERVICE_GET(Mengine::WebViewBridgeServiceInterface))
//////////////////////////////////////////////////////////////////////////
