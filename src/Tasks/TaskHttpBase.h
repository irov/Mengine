#pragma once

#include "Plugins/cURLPlugin/cURLInterface.h"

#include "GOAP/Task.h"

namespace Mengine
{
    /************************************************************************
    uint32_t _status, const String & _error, const String & _response, uint32_t _code, bool _successful
    ************************************************************************/
    /////////////////////////////////////////////////////////////////////////
    typedef Lambda<bool( const GOAP::SourcePtr &, uint32_t, const String &, const String &, uint32_t, bool )> LambdacURLReceiver;
    //////////////////////////////////////////////////////////////////////////
    class TaskHttpBase
        : public GOAP::Task
    {
    public:
        TaskHttpBase( const String & _url, int32_t _timeout, const LambdacURLReceiver & _lambda );
        ~TaskHttpBase() override;

    protected:
        void _onFinally() override;

    protected:
        cURLReceiverInterfacePtr createHttpReceiver_();

    protected:
        void requestComplete_( HttpRequestID _requestId );

    protected:
        String m_url;
        int32_t m_timeout;

        LambdacURLReceiver m_lambda;

        HttpRequestID m_requestId;

        class TaskcURLReceiver;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef GOAP::IntrusivePtr<TaskHttpBase> TaskHttpBasePtr;
    //////////////////////////////////////////////////////////////////////////
}
