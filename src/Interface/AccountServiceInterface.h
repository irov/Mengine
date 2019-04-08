#pragma once

#include "Interface/Interface.h"

#include "Interface/ServiceInterface.h"
#include "Interface/AccountInterface.h"
#include "Interface/AccountProviderInterface.h"

#include "Kernel/ConstString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class AccountVisitorInterface
        : public Interface
    {
    public:
        virtual void onAccount( const AccountInterfacePtr & _account ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<AccountVisitorInterface> AccountVisitorInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class AccountServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "AccountService" )

    public:
        virtual void setAccountProvider( const AccountProviderInterfacePtr & _accountProvider ) = 0;

    public:
        virtual AccountInterfacePtr createAccount() = 0;
        virtual AccountInterfacePtr createGlobalAccount() = 0;

    public:
        virtual bool hasAccount( const ConstString& _accountID ) const = 0;
        virtual void deleteAccount( const ConstString& _accountID ) = 0;
        virtual bool selectAccount( const ConstString& _accountID ) = 0;

    public:
        virtual bool loadAccounts() = 0;
        virtual bool saveAccounts() = 0;

    public:
        virtual void setDefaultAccount( const ConstString & _accountID ) = 0;
        virtual const ConstString & getDefaultAccountID() const = 0;
        virtual bool isCurrentDefaultAccount() const = 0;

        virtual bool hasDefaultAccount() const = 0;

        virtual bool selectDefaultAccount() = 0;

    public:
        virtual void setGlobalAccount( const ConstString & _accountID ) = 0;
        virtual const ConstString & getGlobalAccountID() const = 0;

        virtual bool hasGlobalAccount() const = 0;

    public:
        virtual bool hasCurrentAccount() const = 0;
        virtual const ConstString & getCurrentAccountID() const = 0;

    public:
        virtual const AccountInterfacePtr & getAccount( const ConstString& _accountID ) const = 0;

    public:
        virtual void visitAccounts( const AccountVisitorInterfacePtr & _visitor ) const = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define ACCOUNT_SERVICE()\
    ((AccountServiceInterface*)SERVICE_GET(Mengine::AccountServiceInterface))
//////////////////////////////////////////////////////////////////////////