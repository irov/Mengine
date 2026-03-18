#pragma once

#include "Interface/AccountInterface.h"

#include "Kernel/Mixin.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AccountProviderInterface
        : public Mixin
    {
    public:
        virtual void onCreateAccount( const AccountInterfacePtr & _account, bool _global ) = 0;
        virtual void onDeleteAccount( const AccountInterfacePtr & _account ) = 0;
        virtual void onSelectAccount( const AccountInterfacePtr & _account ) = 0;
        virtual void onUnselectAccount( const AccountInterfacePtr & _account ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AccountProviderInterface> AccountProviderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////    
}