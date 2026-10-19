#pragma once

#include "Interface/LoggerInterface.h"

#if defined(MENGINE_WEBVIEW_MCP)
#   include "Interface/ThreadMutexInterface.h"
#   include "Kernel/JSON.h"
#endif

#include "Environment/Android/AndroidEnv.h"
#include "Environment/Android/AndroidWebViewEvent.h"

namespace Mengine
{
    class AndroidWebViewApplication
    {
    public:
        AndroidWebViewApplication();
        ~AndroidWebViewApplication();

    public:
        bool bootstrap( JNIEnv * _env );
        bool run();
        void finalize();

    protected:
        bool processEvent_( JNIEnv * _env, EAndroidWebViewEvent _id, jobject _event );
        bool registerApplicationHandlers_();
        void publishStatus_();

    protected:
        LoggerInterfacePtr m_logger;

#if defined(MENGINE_WEBVIEW_MCP)
        LoggerInterfacePtr m_logCollector;
        ThreadMutexInterfacePtr m_logMutex;
        jpp::array m_logs;
        uint64_t m_logSequence;
#endif
    };
}
