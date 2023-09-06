#pragma once

#include "Interface/AppleEnvironmentServiceInterface.h"

#include "ConstStringHolderNSString.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Pool.h"
#include "Kernel/IntrusiveList.h"

namespace Mengine
{
    class AppleEnvironmentService
        : public ServiceBase<AppleEnvironmentServiceInterface>
    {
    public:
        AppleEnvironmentService();
        ~AppleEnvironmentService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        void stringize( NSString * _value, ConstString * const _cstr ) override;

    protected:
        typedef Pool<ConstStringHolderNSString, 256> PoolConstStringHolderNSString;
        PoolConstStringHolderNSString m_poolNSString;

        typedef IntrusiveList<ConstStringHolderNSString> IntrusiveListConstStringHolderNSString;
        IntrusiveListConstStringHolderNSString m_holdersNSString;
    };
}
