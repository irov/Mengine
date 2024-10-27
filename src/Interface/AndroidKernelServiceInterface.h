#pragma once

#include "Interface/ServiceInterface.h"

#include "Environment/Android/AndroidIncluder.h"

#include "Kernel/ConstString.h"

namespace Mengine
{
    class AndroidKernelServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "AndroidKernelService" )

    public:
        virtual void stringize( JNIEnv * _jenv, jstring _value, ConstString * const _cstr ) = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define ANDROID_KERNEL_SERVICE()\
    ((Mengine::AndroidKernelServiceInterface *)SERVICE_GET(Mengine::AndroidKernelServiceInterface))
//////////////////////////////////////////////////////////////////////////