#pragma once

#include "Interface/HttpSystemInterface.h"

#include "Environment/Python/PythonIncluder.h"
#include "Environment/Python/PythonCallbackProvider.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class PyHttpReceiver
        : public HttpReceiverInterface
        , public PythonCallbackProvider
        , public Factorable
    {
        DECLARE_FACTORABLE( PyHttpReceiver );

    public:
        PyHttpReceiver();
        ~PyHttpReceiver() override;

    protected:
        void onHttpRequestComplete( const HttpResponseInterfacePtr & _response ) override;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<PyHttpReceiver, HttpReceiverInterface> PyHttpReceiverPtr;
    //////////////////////////////////////////////////////////////////////////
}