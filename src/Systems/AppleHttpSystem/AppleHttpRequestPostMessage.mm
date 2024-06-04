#include "AppleHttpRequestPostMessage.h"

#import "Environment/Apple/AppleString.h"

#include "Kernel/OptionHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/Stringstream.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleHttpRequestPostMessage::AppleHttpRequestPostMessage()
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
            NSString *key = Helper::stringToNSString( property.key );
            NSString *value = Helper::stringToNSString( property.value );
            
            ns_properties[key] = value;
        }

        MengineHttpResponseParam * response = [MengineNetwork httpRequestPostMessage:_request properties:ns_properties];
        
        return response;
    }
    //////////////////////////////////////////////////////////////////////////
}
