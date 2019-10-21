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
    void cURLSource::addHttpGet( const String & _url, int32_t _timeout, const cURLTaskReceiverInterfacePtr & _receiver )
    {
        m_source->addTask<TaskcURLHttpGet>( _url, _timeout, _receiver );
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLSource::addHttpHeaderData( const String & _url, const VectorString & _headers, const String & _data, int32_t _timeout, const cURLTaskReceiverInterfacePtr & _receiver )
    {
        m_source->addTask<TaskcURLHttpHeaderData>( _url, _headers, _data, _timeout, _receiver );
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLSource::addHttpPost( const String & _url, const cURLPostParams & _params, int32_t _timeout, const cURLTaskReceiverInterfacePtr & _receiver )
    {
        m_source->addTask<TaskcURLHttpPost>( _url, _params, _timeout, _receiver );
    }
    //////////////////////////////////////////////////////////////////////////
}