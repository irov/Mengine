#pragma once

#include "Interface/HttpSystemInterface.h"

#include "Environment/Python/PythonIncluder.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class PyHttpReceiver
        : public HttpReceiverInterface
        , public Factorable
    {
        DECLARE_FACTORABLE( PyHttpReceiver );

    public:
        PyHttpReceiver();
        ~PyHttpReceiver() override;

    public:
        bool initialize( const pybind::object & _cb, const pybind::args & _args );

    protected:
        void onHttpRequestComplete( const HttpResponseInterfacePtr & _response ) override;

    protected:
        pybind::object m_cb;
        pybind::args m_args;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<PyHttpReceiver, HttpReceiverInterface> PyHttpReceiverPtr;
    //////////////////////////////////////////////////////////////////////////
}