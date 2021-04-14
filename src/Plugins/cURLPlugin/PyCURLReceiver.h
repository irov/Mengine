#pragma once

#include "cURLInterface.h"

#include "Kernel/Factorable.h"

#include "pybind/pybind.hpp"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class PyCURLReceiver
        : public Factorable
        , public cURLReceiverInterface
    {
        DECLARE_FACTORABLE( PyCURLReceiver );

    public:
        PyCURLReceiver();
        ~PyCURLReceiver() override;

    public:
        bool initialize( const pybind::object & _cb, const pybind::args & _args );

    protected:
        void onHttpRequestComplete( HttpRequestID _id, uint32_t _status, const String & _error, const cURLHeaders & _headers, const String & _response, uint32_t _code, bool _successful ) override;

    protected:
        pybind::object m_cb;
        pybind::args m_args;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<PyCURLReceiver> PyCURLReceiverPtr;
    //////////////////////////////////////////////////////////////////////////
}