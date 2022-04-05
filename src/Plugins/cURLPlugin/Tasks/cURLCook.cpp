#include "cURLCook.h"

#include "Plugins/GOAPPlugin/Tasks/GOAPCook.h"

#include "TaskcURLHttpGet.h"
#include "TaskcURLHttpHeaderData.h"
#include "TaskcURLHttpPost.h"

namespace Mengine
{
    namespace Cook
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
                void onResponse( const GOAP::SourceInterfacePtr & _source, const cURLResponseData & _response ) override
                {
                    m_f( _source, _response.status, _response.error, _response.headers, _response.data, _response.code, _response.successful );
                }

            protected:
                LambdaTaskReceiver m_f;
            };
            //////////////////////////////////////////////////////////////////////////
        }
        //////////////////////////////////////////////////////////////////////////
        void addHttpGet( const GOAP::SourceInterfacePtr & _source, const String & _url, const cURLHeaders & _headers, int32_t _timeout, bool _receiveHeaders, const LambdaTaskReceiver & _lambda, const DocumentPtr & _doc )
        {
            cURLTaskReceiverInterfacePtr receiver = Helper::makeFactorableUnique<Detail::cURLTaskReceiverF>( _doc, _lambda );

            Cook::addTask<TaskcURLHttpGet>( _source, _url, _headers, _timeout, _receiveHeaders, receiver );
        }
        //////////////////////////////////////////////////////////////////////////
        void addHttpHeaderData( const GOAP::SourceInterfacePtr & _source, const String & _url, const cURLHeaders & _headers, int32_t _timeout, bool _receiveHeaders, const String & _data, const LambdaTaskReceiver & _lambda, const DocumentPtr & _doc )
        {
            cURLTaskReceiverInterfacePtr receiver = Helper::makeFactorableUnique<Detail::cURLTaskReceiverF>( _doc, _lambda );

            Cook::addTask<TaskcURLHttpHeaderData>( _source, _url, _headers, _timeout, _receiveHeaders, _data, receiver );
        }
        //////////////////////////////////////////////////////////////////////////
        void addHttpPost( const GOAP::SourceInterfacePtr & _source, const String & _url, const cURLHeaders & _headers, int32_t _timeout, bool _receiveHeaders, const cURLPostParams & _params, const LambdaTaskReceiver & _lambda, const DocumentPtr & _doc )
        {
            cURLTaskReceiverInterfacePtr receiver = Helper::makeFactorableUnique<Detail::cURLTaskReceiverF>( _doc, _lambda );

            Cook::addTask<TaskcURLHttpPost>( _source, _url, _headers, _timeout, _receiveHeaders, _params, receiver );
        }
        //////////////////////////////////////////////////////////////////////////
    }
}