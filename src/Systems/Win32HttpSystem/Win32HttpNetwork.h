#pragma once

#include "Interface/HttpRequestInterface.h"
#include "Interface/HttpResponseInterface.h"

#include "Kernel/String.h"
#include "Kernel/Data.h"
#include "Kernel/VectorString.h"

namespace Mengine
{
    namespace Helper
    {
        bool httpRequestPostMessage( const HttpRequestInterface * _request, const HttpResponseInterfacePtr & _response, const HttpRequestPostProperties & _properties );
        bool httpRequestHeaderData( const HttpRequestInterface * _request, const HttpResponseInterfacePtr & _response, const Data & _data );
        bool httpRequestGetMessage( const HttpRequestInterface * _request, const HttpResponseInterfacePtr & _response );
        bool httpRequestDeleteMessage( const HttpRequestInterface * _request, const HttpResponseInterfacePtr & _response );
        bool httpRequestGetAsset( const HttpRequestInterface * _request, const HttpResponseInterfacePtr & _response, const String & _login, const String & _password );
    };
}