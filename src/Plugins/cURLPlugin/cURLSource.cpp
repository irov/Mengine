#include "cURLSource.h"

#include "TaskcURLHttpGet.h"
#include "TaskcURLHttpHeaderData.h"
#include "TaskcURLHttpPost.h"

namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        class cURLTaskReceiverF
            : public cURLTaskReceiverInterface
        {
        public:
            cURLTaskReceiverF( const LambdaTaskReceiver & _f )
                : m_f( _f )
            {
            }

            ~cURLTaskReceiverF() override
            {
            }

        protected:
            void onResponse( const EngineSourcePtr & _source, uint32_t _status, const String & _error, const cURLHeaders & _headers, const String & _response, uint32_t _code, bool _successful ) override
            {
                m_f( _source, _status, _error, _headers, _response, _code, _successful );
            }

        protected:
            LambdaTaskReceiver m_f;
        };
    }
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
    void cURLSource::addHttpGet( const String & _url, const cURLHeaders & _headers, int32_t _timeout, bool _receiveHeaders, const LambdaTaskReceiver & _lambda, const DocumentPtr & _doc )
    {
        cURLTaskReceiverInterfacePtr receiver = Helper::makeFactorableUnique<Detail::cURLTaskReceiverF>( _doc, _lambda );

        this->addHttpGet_( _url, _headers, _timeout, _receiveHeaders, receiver );
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLSource::addHttpHeaderData( const String & _url, const cURLHeaders & _headers, int32_t _timeout, bool _receiveHeaders, const String & _data, const LambdaTaskReceiver & _lambda, const DocumentPtr & _doc )
    {
        cURLTaskReceiverInterfacePtr receiver = Helper::makeFactorableUnique<Detail::cURLTaskReceiverF>( _doc, _lambda );

        this->addHttpHeaderData_( _url, _headers, _timeout, _receiveHeaders, _data, receiver );
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLSource::addHttpPost( const String & _url, const cURLHeaders & _headers, int32_t _timeout, bool _receiveHeaders, const cURLPostParams & _params, const LambdaTaskReceiver & _lambda, const DocumentPtr & _doc )
    {
        cURLTaskReceiverInterfacePtr receiver = Helper::makeFactorableUnique<Detail::cURLTaskReceiverF>( _doc, _lambda );

        this->addHttpPost_( _url, _headers, _timeout, _receiveHeaders, _params, receiver );
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLSource::addHttpGet_( const String & _url, const cURLHeaders & _headers, int32_t _timeout, bool _receiveHeaders, const cURLTaskReceiverInterfacePtr & _receiver )
    {
        m_source->addTask<TaskcURLHttpGet>( _url, _headers, _timeout, _receiveHeaders, _receiver );
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLSource::addHttpHeaderData_( const String & _url, const cURLHeaders & _headers, int32_t _timeout, bool _receiveHeaders, const String & _data, const cURLTaskReceiverInterfacePtr & _receiver )
    {
        m_source->addTask<TaskcURLHttpHeaderData>( _url, _headers, _timeout, _receiveHeaders, _data, _receiver );
    }
    //////////////////////////////////////////////////////////////////////////
    void cURLSource::addHttpPost_( const String & _url, const cURLHeaders & _headers, int32_t _timeout, bool _receiveHeaders, const cURLPostParams & _params, const cURLTaskReceiverInterfacePtr & _receiver )
    {
        m_source->addTask<TaskcURLHttpPost>( _url, _headers, _timeout, _receiveHeaders, _params, _receiver );
    }
    //////////////////////////////////////////////////////////////////////////
}