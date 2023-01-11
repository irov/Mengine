#pragma once

#include "Environment/Android/AndroidIncluder.h"

#include "Kernel/LoggerBase.h"
#include "Kernel/String.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AndroidProxyLogger
        : public LoggerBase
    {
    public:
        AndroidProxyLogger();
        ~AndroidProxyLogger() override;

    public:
        void setJNIEnv( JNIEnv * _jenv );
        JNIEnv * getJNIEnv() const;

        void setJClassMengineActivity( jclass _jclassMengineActivity );
        jclass getJClassMengineActivity() const;

        void setJObjectMengineActivity( jobject _jobjectMengineActivity );
        jobject getJObjectMengineActivity() const;

    protected:
        bool _initializeLogger() override;
        void _finalizeLogger() override;

    protected:
        void log( const LoggerMessage & _message ) override;

    protected:
        JNIEnv * m_jenv;

        jclass m_jclassMengineActivity;
        jobject m_jobjectMengineActivity;

        Char m_loggerMessage[MENGINE_LOGGER_MAX_MESSAGE];
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AndroidProxyLogger, LoggerInterface> AndroidProxyLoggerPtr;
    //////////////////////////////////////////////////////////////////////////
}
