#include "AppleHttpRequestGetMessage.h"

#include "Kernel/OptionHelper.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleHttpRequestGetMessage::AppleHttpRequestGetMessage()
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
