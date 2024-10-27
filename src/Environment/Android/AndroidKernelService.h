#pragma once

#include "Interface/AndroidKernelServiceInterface.h"
#include "Interface/ThreadMutexInterface.h"

#include "ConstStringHolderJString.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Pool.h"
#include "Kernel/IntrusiveList.h"

namespace Mengine
{
    class AndroidKernelService
        : public ServiceBase<AndroidKernelServiceInterface>
    {
    public:
        AndroidKernelService();
        ~AndroidKernelService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        void stringize( JNIEnv * _jenv, jstring _value, ConstString * const _cstr ) override;

    protected:
        ThreadMutexInterfacePtr m_mutexJStrings;

        typedef Pool<ConstStringHolderJString, 256> PoolConstStringHolderJString;
        PoolConstStringHolderJString m_poolJString;

        typedef IntrusiveList<ConstStringHolderJString> IntrusiveListConstStringHolderJString;
        IntrusiveListConstStringHolderJString m_holdersJString;
    };
}
