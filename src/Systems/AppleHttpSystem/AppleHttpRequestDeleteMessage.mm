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
    AppleHttpResponseParam * AppleHttpRequestDeleteMessage::_onHttp( AppleHttpRequestParam * _request )
    {
        AppleHttpResponseParam * response = [AppleHttpNetwork httpRequestDeleteMessage:_request];
        
        return response;
    }
    //////////////////////////////////////////////////////////////////////////
}
