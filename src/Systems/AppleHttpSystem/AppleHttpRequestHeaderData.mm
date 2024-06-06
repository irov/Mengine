#include "AppleHttpRequestHeaderData.h"

#import "Environment/Apple/AppleDetail.h"

#include "Kernel/OptionHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/Stringstream.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AppleHttpRequestHeaderData::AppleHttpRequestHeaderData()
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
    MengineHttpResponseParam * AppleHttpRequestHeaderData::_onHttp( MengineHttpRequestParam * _request )
    {
        NSData * ns_data = Helper::AppleGetNSDataFromData( m_data );
        
        MengineHttpResponseParam * response = [MengineHttpNetwork httpRequestHeaderData:_request data:ns_data];
        
        return response;
    }
    //////////////////////////////////////////////////////////////////////////
}
