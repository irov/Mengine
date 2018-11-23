#pragma once

#include "Kernel/Mixin.h"
#include "Kernel/ConstString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AccountProviderInterface
        : public Mixin
    {
    public:
        virtual void onCreateAccount( const ConstString & _accountID, bool _global ) = 0;
        virtual void onDeleteAccount( const ConstString & _accountID ) = 0;
        virtual void onSelectAccount( const ConstString & _accountID ) = 0;
        virtual void onUnselectAccount( const ConstString & _accountID ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AccountProviderInterface> AccountProviderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////    
}