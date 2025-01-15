#include "AppleHttpRequestGetMessage.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleHttpRequestGetMessage::AppleHttpRequestGetMessage()
        : AppleHttpRequest( true )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleHttpRequestGetMessage::~AppleHttpRequestGetMessage()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleHttpResponseParam * AppleHttpRequestGetMessage::_onHttp( AppleHttpRequestParam * _request )
    {
        AppleHttpResponseParam * response = [AppleHttpNetwork httpRequestGetMessage:_request];
        
        return response;
    }
    //////////////////////////////////////////////////////////////////////////
}
