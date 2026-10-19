#pragma once

#include "Interface/WebViewBootstrapperInterface.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/UpdateContext.h"
#include "Kernel/Vector.h"

namespace Mengine
{
    class WebViewBootstrapper
        : public ServiceBase<WebViewBootstrapperInterface>
    {
        DECLARE_FACTORABLE( WebViewBootstrapper );

    public:
        WebViewBootstrapper();
        ~WebViewBootstrapper() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        bool initialize() override;
        bool run() override;
        void tick( float _time ) override;
        void stop() override;
        void finalize() override;

    protected:
        bool createServices_();
        bool createNetworkServices_();

    protected:
        typedef Vector<const Char *> VectorServiceNames;

        enum EState
        {
            STATE_CREATED,
            STATE_INITIALIZED,
            STATE_RUNNING,
            STATE_STOPPED
        };

    protected:
        VectorServiceNames m_services;
        UpdateContext m_context;
        EState m_state;
    };
}
