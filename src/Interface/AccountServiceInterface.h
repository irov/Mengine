#pragma once

#include "Interface/Interface.h"

#include "Interface/ServiceInterface.h"
#include "Interface/AccountInterface.h"
#include "Interface/AccountProviderInterface.h"

#include "Kernel/ConstString.h"

namespace Mengine
{
    class AccountServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "AccountService" )

    public:
        virtual void setAccountProvider( const AccountProviderInterfacePtr & _accountProvider ) = 0;

    public:
        virtual AccountInterfacePtr createAccount( const DocumentPtr & _doc ) = 0;
        virtual AccountInterfacePtr createGlobalAccount( const DocumentPtr & _doc ) = 0;

    public:
        virtual bool hasAccount( const ConstString & _accountID ) const = 0;
        virtual void deleteAccount( const ConstString & _accountID ) = 0;
        virtual bool selectAccount( const ConstString & _accountID ) = 0;

    public:
        virtual bool loadAccounts() = 0;
        virtual bool saveAccounts() = 0;

    public:
        virtual void stopAccounts() = 0;

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
        virtual const AccountInterfacePtr & getAccount( const ConstString & _accountID ) const = 0;

    public:
        typedef Lambda<void( const AccountInterfacePtr & )> LambdaAccounts;
        virtual void foreachAccounts( const LambdaAccounts & _lambda ) const = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define ACCOUNT_SERVICE()\
    ((Mengine::AccountServiceInterface*)SERVICE_GET(Mengine::AccountServiceInterface))
//////////////////////////////////////////////////////////////////////////