#include "cURLSource.h"

#include "TaskcURLHttpGet.h"
#include "TaskcURLHttpHeaderData.h"
#include "TaskcURLHttpPost.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    cURLSource::cURLSource()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    cURLSource::~cURLSource()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLSource::setSource( const EngineSourcePtr & _source )
    {
        m_source = _source;
    }
    //////////////////////////////////////////////////////////////////////////
    const EngineSourcePtr & cURLSource::getSource() const
    {
        return m_source;
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLSource::addHttpGet( const String & _url, const cURLHeaders & _headers, int32_t _timeout, bool _receiveHeaders, const cURLTaskReceiverInterfacePtr & _receiver )
    {
        m_source->addTask<TaskcURLHttpGet>( _url, _headers, _timeout, _receiveHeaders, _receiver );
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLSource::addHttpHeaderData( const String & _url, const cURLHeaders & _headers, int32_t _timeout, bool _receiveHeaders, const String & _data, const cURLTaskReceiverInterfacePtr & _receiver )
    {
        m_source->addTask<TaskcURLHttpHeaderData>( _url, _headers, _timeout, _receiveHeaders, _data, _receiver );
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLSource::addHttpPost( const String & _url, const cURLHeaders & _headers, int32_t _timeout, bool _receiveHeaders, const String & _cookies, const cURLPostParams & _params, const cURLTaskReceiverInterfacePtr & _receiver )
    {
        m_source->addTask<TaskcURLHttpPost>( _url, _headers, _timeout, _receiveHeaders, _cookies, _params, _receiver );
    }
    //////////////////////////////////////////////////////////////////////////
}