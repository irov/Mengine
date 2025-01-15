#include "AppleHttpRequestPing.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleHttpRequestPing::AppleHttpRequestPing()
        : AppleHttpRequest( true )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleHttpRequestPing::~AppleHttpRequestPing()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleHttpResponseParam * AppleHttpRequestPing::_onHttp( AppleHttpRequestParam * _request )
    {
        AppleHttpResponseParam * response = [AppleHttpNetwork httpRequestPing:_request];
        
        return response;
    }
    //////////////////////////////////////////////////////////////////////////
}
