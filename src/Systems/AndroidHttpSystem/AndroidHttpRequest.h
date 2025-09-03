#pragma once

#include "Environment/Android/AndroidEnv.h"

#include "Kernel/BaseHttpRequest.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AndroidHttpRequest
        : public BaseHttpRequest
    {
    public:
        AndroidHttpRequest( bool _isJSON );
        ~AndroidHttpRequest() override;

    protected:
        void _onThreadTaskPreparation() override;
        bool _onThreadTaskRun() override;
        bool _onThreadTaskProcess() override;

    protected:
        virtual jobject _onHttp( JNIEnv * _jenv, jobject _basses ) = 0;

    protected:
        void _onThreadTaskComplete( bool _successful ) override;

    protected:
        bool m_isJSON;

        ThreadMutexInterfacePtr m_mutex;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AndroidHttpRequest, ThreadTaskInterface> AndroidHttpRequestPtr;
    //////////////////////////////////////////////////////////////////////////
}