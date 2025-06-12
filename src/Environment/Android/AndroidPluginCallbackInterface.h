#pragma once

#include "Interface/ServantInterface.h"

#include "Environment/Android/AndroidIncluder.h"

namespace Mengine
{
    ////////////////////////////////////////////////////////////////////
    class AndroidPluginCallbackInterface
        : public ServantInterface
    {
    public:
        virtual void invoke( JNIEnv * _jenv, jobjectArray _args ) = 0;
    };
    ////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AndroidPluginCallbackInterface> AndroidPluginCallbackInterfacePtr;
    ////////////////////////////////////////////////////////////////////
}