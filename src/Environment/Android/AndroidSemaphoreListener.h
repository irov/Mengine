#pragma once

#include "Interface/SemaphoreListenerInterface.h"

#include "Environment/Android/AndroidIncluder.h"

#include "Kernel/Factorable.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AndroidSemaphoreListener
        : public SemaphoreListenerInterface
        , public Factorable
    {
    public:
        AndroidSemaphoreListener( JNIEnv * _jenv, jobject _cb );
        ~AndroidSemaphoreListener() override;

    public:
        void onActivated() override;

    protected:
        jobject m_cb;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AndroidSemaphoreListener, SemaphoreListenerInterface> AndroidSemaphoreListenerPtr;
    //////////////////////////////////////////////////////////////////////////
}
