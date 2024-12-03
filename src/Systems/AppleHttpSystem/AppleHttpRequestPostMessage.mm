#include "AppleHttpRequestPostMessage.h"

#import "Environment/Apple/AppleString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleHttpRequestPostMessage::AppleHttpRequestPostMessage()
        : AppleHttpRequest( true )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleHttpRequestPostMessage::~AppleHttpRequestPostMessage()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleHttpRequestPostMessage::setPostProperties( const HttpRequestPostProperties & _properties )
    {
        m_properties = _properties;
    }
    //////////////////////////////////////////////////////////////////////////
    const HttpRequestPostProperties & AppleHttpRequestPostMessage::getPostProperties() const
    {
        return m_properties;
    }
    //////////////////////////////////////////////////////////////////////////
    MengineHttpResponseParam * AppleHttpRequestPostMessage::_onHttp( MengineHttpRequestParam * _request )
    {
        NSMutableDictionary<NSString *, NSString *> * ns_properties = [NSMutableDictionary dictionary];
            
        for( const HttpRequestPostProperty & property : m_properties )
        {
            NSString * key = [AppleString NSStringFromConstString:property.key];
            NSString * value = [AppleString NSStringFromString:property.value];
            
            ns_properties[key] = value;
        }

        MengineHttpResponseParam * response = [MengineHttpNetwork httpRequestPostMessage:_request properties:ns_properties];
        
        return response;
    }
    //////////////////////////////////////////////////////////////////////////
}
