#pragma once

#include "Interface/ServiceInterface.h"
#include "Interface/ContentInterface.h"

#include "Interface/HttpRequestInterface.h"

#include "Kernel/Data.h"

namespace Mengine
{
    class HttpSystemInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "HttpSystem" );

    public:
        virtual HttpRequestInterfacePtr createHttpRequestPing( const DocumentInterfacePtr & _doc ) = 0;

    public:
        virtual HttpRequestInterfacePtr createHttpRequestGetMessage( const DocumentInterfacePtr & _doc ) = 0;
        virtual HttpRequestInterfacePtr createHttpRequestPostMessage( const HttpRequestPostProperties & _params, const DocumentInterfacePtr & _doc ) = 0;
        virtual HttpRequestInterfacePtr createHttpRequestHeaderData( const Data & _data, const DocumentInterfacePtr & _doc ) = 0;
        virtual HttpRequestInterfacePtr createHttpRequestGetAsset( const String & _login, const String & _password, const ContentInterfacePtr & _content, const DocumentInterfacePtr & _doc ) = 0;
        virtual HttpRequestInterfacePtr createHttpRequestDeleteMessage( const DocumentInterfacePtr & _doc ) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define HTTP_SYSTEM()\
    ((Mengine::HttpSystemInterface *)SERVICE_GET(Mengine::HttpSystemInterface))
//////////////////////////////////////////////////////////////////////////
