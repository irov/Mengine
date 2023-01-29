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
        virtual void onCreateAccount( const ConstString & _accountId, bool _global ) = 0;
        virtual void onDeleteAccount( const ConstString & _accountId ) = 0;
        virtual void onSelectAccount( const ConstString & _accountId ) = 0;
        virtual void onUnselectAccount( const ConstString & _accountId ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AccountProviderInterface> AccountProviderInterfacePtr;
    //////////////////////////////////////////////////////////////////////////    
}