#pragma once

#include "cURLInterface.h"

#include "Environment/Python/PythonIncluder.h"

#include "Kernel/Factorable.h"

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
        void onHttpRequestComplete( const cURLResponseData & _response ) override;

    protected:
        pybind::object m_cb;
        pybind::args m_args;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<PyCURLReceiver> PyCURLReceiverPtr;
    //////////////////////////////////////////////////////////////////////////
}