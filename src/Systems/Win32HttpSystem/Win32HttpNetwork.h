#pragma once

#include "Win32HttpRequest.h"

#include "Kernel/String.h"
#include "Kernel/Data.h"
#include "Kernel/VectorString.h"

namespace Mengine
{
    namespace Helper
    {
        bool httpRequestPing( const Win32HttpRequest * _request, const HttpResponseInterfacePtr & _response );
        bool httpRequestPostMessage( const Win32HttpRequest * _request, const HttpResponseInterfacePtr & _response, const HttpRequestPostProperties & _properties );
        bool httpRequestHeaderData( const Win32HttpRequest * _request, const HttpResponseInterfacePtr & _response, const Data & _data );
        bool httpRequestGetMessage( const Win32HttpRequest * _request, const HttpResponseInterfacePtr & _response );
        bool httpRequestDeleteMessage( const Win32HttpRequest * _request, const HttpResponseInterfacePtr & _response );
        bool httpRequestGetAsset( const Win32HttpRequest * _request, const HttpResponseInterfacePtr & _response, const String & _login, const String & _password );
    };
}