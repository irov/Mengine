#include "AppleHttpRequestHeaderData.h"

#import "Environment/Apple/AppleDetail.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleHttpRequestHeaderData::AppleHttpRequestHeaderData()
        : AppleHttpRequest( true )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AppleHttpRequestHeaderData::~AppleHttpRequestHeaderData()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void AppleHttpRequestHeaderData::setData( const Data & _data )
    {
        m_data = _data;
    }
    //////////////////////////////////////////////////////////////////////////
    const Data & AppleHttpRequestHeaderData::getData() const
    {
        return m_data;
    }
    //////////////////////////////////////////////////////////////////////////
    AppleHttpResponseParam * AppleHttpRequestHeaderData::_onHttp( AppleHttpRequestParam * _request )
    {
        NSData * ns_data = [AppleDetail getNSDataFromData:m_data];
        
        AppleHttpResponseParam * response = [AppleHttpNetwork httpRequestHeaderData:_request data:ns_data];
        
        return response;
    }
    //////////////////////////////////////////////////////////////////////////
}
