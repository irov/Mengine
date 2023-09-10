#include "HttpCook.h"

#include "Plugins/GOAPPlugin/Tasks/GOAPCook.h"

#include "TaskHttpGet.h"
#include "TaskHttpHeaderData.h"
#include "TaskHttpPost.h"

namespace Mengine
{
    namespace Cook
    {
        namespace Detail
        {
            //////////////////////////////////////////////////////////////////////////
            class HttpTaskReceiverF
                : public HttpTaskReceiverInterface
            {
            public:
                explicit HttpTaskReceiverF( const LambdaTaskReceiver & _f )
                    : m_f( _f )
                {
                }

                ~HttpTaskReceiverF() override
                {
                }

            protected:
                void onResponse( const GOAP::SourceInterfacePtr & _source, const HttpResponseInterfacePtr & _response ) override
                {
                    m_f( _source, _response );
                }

            protected:
                LambdaTaskReceiver m_f;
            };
            //////////////////////////////////////////////////////////////////////////
        }
        //////////////////////////////////////////////////////////////////////////
        void addHttpGet( const GOAP::SourceInterfacePtr & _source, const String & _url, const HttpRequestHeaders & _headers, int32_t _timeout, bool _receiveHeaders, const LambdaTaskReceiver & _lambda, const DocumentInterfacePtr & _doc )
        {
            HttpTaskReceiverInterfacePtr receiver = Helper::makeFactorableUnique<Detail::HttpTaskReceiverF>( _doc, _lambda );

            Cook::addTask<TaskHttpGet>( _source, _url, _headers, _timeout, _receiveHeaders, receiver );
        }
        //////////////////////////////////////////////////////////////////////////
        void addHttpHeaderData( const GOAP::SourceInterfacePtr & _source, const String & _url, const HttpRequestHeaders & _headers, int32_t _timeout, bool _receiveHeaders, const String & _data, const LambdaTaskReceiver & _lambda, const DocumentInterfacePtr & _doc )
        {
            HttpTaskReceiverInterfacePtr receiver = Helper::makeFactorableUnique<Detail::HttpTaskReceiverF>( _doc, _lambda );

            Cook::addTask<TaskHttpHeaderData>( _source, _url, _headers, _timeout, _receiveHeaders, _data, receiver );
        }
        //////////////////////////////////////////////////////////////////////////
        void addHttpPost( const GOAP::SourceInterfacePtr & _source, const String & _url, const HttpRequestHeaders & _headers, int32_t _timeout, bool _receiveHeaders, const HttpRequestPostParams & _params, const LambdaTaskReceiver & _lambda, const DocumentInterfacePtr & _doc )
        {
            HttpTaskReceiverInterfacePtr receiver = Helper::makeFactorableUnique<Detail::HttpTaskReceiverF>( _doc, _lambda );

            Cook::addTask<TaskHttpPost>( _source, _url, _headers, _timeout, _receiveHeaders, _params, receiver );
        }
        //////////////////////////////////////////////////////////////////////////
    }
}