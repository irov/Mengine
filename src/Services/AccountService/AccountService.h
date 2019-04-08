#pragma once

#include "Interface/AccountServiceInterface.h"
#include "Interface/ArchivatorInterface.h"
#include "Interface/FileGroupInterface.h"

#include "Kernel/ServiceBase.h"

#include "Kernel/Factory.h"

#include "Config/Map.h"

namespace Mengine
{
    class AccountService
        : public ServiceBase<AccountServiceInterface>
    {
    public:
        AccountService();
        ~AccountService() override;

    public:
        bool _initializeService() override;
        void _finalizeService() override;

    public:
        void setAccountProvider( const AccountProviderInterfacePtr & _accountProvider ) override;

    public:
        AccountInterfacePtr createAccount() override;
        AccountInterfacePtr createGlobalAccount() override;

    public:
        bool hasAccount( const ConstString& _accountID ) const override;
        void deleteAccount( const ConstString& _accountID ) override;
        bool selectAccount( const ConstString& _accountID ) override;

    public:
        bool loadAccounts() override;
        bool saveAccounts() override;

    public:
        void setDefaultAccount( const ConstString & _accountID ) override;
        const ConstString & getDefaultAccountID() const override;

        bool isCurrentDefaultAccount() const override;

        bool hasDefaultAccount() const override;

        bool selectDefaultAccount() override;

    public:
        void setGlobalAccount( const ConstString & _accountID ) override;
        const ConstString & getGlobalAccountID() const override;

        bool hasGlobalAccount() const override;

    public:
        bool hasCurrentAccount() const override;
        const ConstString & getCurrentAccountID() const override;

    public:
        const AccountInterfacePtr & getAccount( const ConstString& _accountID ) const override;

        void visitAccounts( const AccountVisitorInterfacePtr & _visitor ) const override;

    protected:
        bool loadAccount_( const AccountInterfacePtr & _account );
        AccountInterfacePtr createAccount_( const ConstString& _accountID );
        AccountInterfacePtr createGlobalAccount_( const ConstString& _accountID );

    protected:
        AccountInterfacePtr newAccount_( const ConstString& _accountID );

    protected:
        void unselectCurrentAccount_();

    protected:
        AccountProviderInterfacePtr m_accountProvider;

        FileGroupInterfacePtr m_fileGroup;
        ArchivatorInterfacePtr m_archivator;

        typedef Map<ConstString, AccountInterfacePtr> MapAccounts;
        MapAccounts m_accounts;

        FactoryPtr m_factoryAccounts;

        ConstString m_currentAccountID;
        ConstString m_globalAccountID;
        ConstString m_defaultAccountID;

        uint32_t m_playerEnumerator;

        bool m_invalidateAccounts;
    };
}