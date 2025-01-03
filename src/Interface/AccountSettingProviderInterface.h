#pragma once

#include "Kernel/Mixin.h"

#include "Config/Char.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AccountSettingProviderInterface
        : public Mixin
    {
    public:
        virtual void onChangeSetting( const Char * _value ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AccountSettingProviderInterface> AccountSettingProviderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}