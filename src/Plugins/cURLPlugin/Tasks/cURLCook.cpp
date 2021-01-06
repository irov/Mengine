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
                void onResponse( const GOAP::SourceInterfacePtr & _source, uint32_t _status, const String & _error, const cURLHeaders & _headers, const String & _response, uint32_t _code, bool _successful ) override
                {
                    m_f( _source, _status, _error, _headers, _response, _code, _successful );
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