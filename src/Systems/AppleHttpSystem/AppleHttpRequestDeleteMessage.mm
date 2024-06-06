#include "AppleHttpRequestDeleteMessage.h"

#include "Kernel/OptionHelper.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleHttpRequestDeleteMessage::AppleHttpRequestDeleteMessage()
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
