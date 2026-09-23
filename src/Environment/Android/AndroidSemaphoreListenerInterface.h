#pragma once

#include "Interface/ServantInterface.h"

#include "Environment/Android/AndroidEnv.h"

namespace Mengine
{
    ////////////////////////////////////////////////////////////////////
    class AndroidSemaphoreListenerInterface
        : public ServantInterface
    {
    public:
        virtual void invoke( JNIEnv * _jenv, jobject _value ) = 0;
    };
    ////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AndroidSemaphoreListenerInterface> AndroidSemaphoreListenerInterfacePtr;
    ////////////////////////////////////////////////////////////////////
}