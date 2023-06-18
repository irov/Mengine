#pragma once

#include "Interface/HttpSystemInterface.h"

#include "Plugins/GOAPPlugin/GOAPInterface.h"

#include "Kernel/String.h"
#include "Kernel/VectorString.h"

#include "Config/Lambda.h"
#include "Config/Export.h"

namespace Mengine
{
    namespace Cook
    {
        //////////////////////////////////////////////////////////////////////////
        typedef Lambda<void( const GOAP::SourceInterfacePtr & _source, const HttpResponseInterfacePtr & _response )> LambdaTaskReceiver;
        //////////////////////////////////////////////////////////////////////////
        MENGINE_DLL_EXPORT void addHttpGet( const GOAP::SourceInterfacePtr & _source, const String & _url, const HttpRequestHeaders & _headers, int32_t _timeout, bool _receiveHeaders, const LambdaTaskReceiver & _lambda, const DocumentPtr & _doc );
        MENGINE_DLL_EXPORT void addHttpHeaderData( const GOAP::SourceInterfacePtr & _source, const String & _url, const HttpRequestHeaders & _headers, int32_t _timeout, bool _receiveHeaders, const String & _data, const LambdaTaskReceiver & _lambda, const DocumentPtr & _doc );
        MENGINE_DLL_EXPORT void addHttpPost( const GOAP::SourceInterfacePtr & _source, const String & _url, const HttpRequestHeaders & _headers, int32_t _timeout, bool _receiveHeaders, const HttpRequestPostParams & _params, const LambdaTaskReceiver & _lambda, const DocumentPtr & _doc );
        //////////////////////////////////////////////////////////////////////////
    }
}