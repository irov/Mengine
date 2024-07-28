#pragma once

#include "Interface/Interface.h"

#include "Config/Char.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AccountSettingProviderInterface
        : public Interface
    {
    public:
        virtual void onChangeSetting( const Char * _value ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AccountSettingProviderInterface> AccountSettingProviderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}