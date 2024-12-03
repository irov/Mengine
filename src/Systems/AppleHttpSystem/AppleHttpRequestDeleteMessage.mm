#include "AppleHttpRequestDeleteMessage.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleHttpRequestDeleteMessage::AppleHttpRequestDeleteMessage()
        : AppleHttpRequest( true )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleHttpRequestDeleteMessage::~AppleHttpRequestDeleteMessage()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MengineHttpResponseParam * AppleHttpRequestDeleteMessage::_onHttp( MengineHttpRequestParam * _request )
    {
        MengineHttpResponseParam * response = [MengineHttpNetwork httpRequestDeleteMessage:_request];
        
        return response;
    }
    //////////////////////////////////////////////////////////////////////////
}
