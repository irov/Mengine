#pragma once

#include "Interface/Interface.h"

#include "Kernel/ConstString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AccountProviderInterface
        : public Interface
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