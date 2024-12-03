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
    MengineHttpResponseParam * AppleHttpRequestGetMessage::_onHttp( MengineHttpRequestParam * _request )
    {
        MengineHttpResponseParam * response = [MengineHttpNetwork httpRequestGetMessage:_request];
        
        return response;
    }
    //////////////////////////////////////////////////////////////////////////
}
