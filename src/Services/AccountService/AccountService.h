#pragma once

#include "Interface/AccountServiceInterface.h"
#include "Interface/ArchivatorInterface.h"
#include "Interface/FileGroupInterface.h"
#include "Interface/FactoryInterface.h"

#include "Account.h"

#include "Kernel/ServiceBase.h"
#include "Kernel/Hashtable.h"

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
        AccountInterfacePtr createAccount( const DocumentInterfacePtr & _doc ) override;
        AccountInterfacePtr createGlobalAccount( const DocumentInterfacePtr & _doc ) override;

    public:
        bool hasAccount( const ConstString & _accountId ) const override;
        bool deleteAccount( const ConstString & _accountId ) override;
        bool selectAccount( const ConstString & _accountId ) override;

    public:
        bool loadAccounts() override;
        bool saveAccounts() override;

    protected:
        ConfigInterfacePtr getLoadSettingConfig_() const;

    public:
        void stopAccounts() override;

    public:
        void setDefaultAccount( const ConstString & _accountId ) override;
        const ConstString & getDefaultAccountId() const override;

        bool isCurrentDefaultAccount() const override;

        bool hasDefaultAccount() const override;
        bool selectDefaultAccount() override;

    public:
        void setGlobalAccount( const ConstString & _accountId ) override;
        const ConstString & getGlobalAccountId() const override;

        bool hasGlobalAccount() const override;

    public:
        bool hasCurrentAccount() const override;
        bool deleteCurrentAccount() override;

        const ConstString & getCurrentAccountId() const override;
        const AccountInterfacePtr & getCurrentAccount() const override;

    public:
        const AccountInterfacePtr & getAccount( const ConstString & _accountId ) const override;

    public:
        void foreachAccounts( const LambdaAccounts & _lambda ) const override;

    protected:
        bool loadAccount_( const AccountInterfacePtr & _account );
        AccountInterfacePtr createAccount_( const ConstString & _accountId, const DocumentInterfacePtr & _doc );
        AccountInterfacePtr createGlobalAccount_( const ConstString & _accountId, const DocumentInterfacePtr & _doc );

    protected:
        AccountInterfacePtr newAccount_( const ConstString & _accountId, const DocumentInterfacePtr & _doc );

    protected:
        void unselectCurrentAccount_();

    protected:
        AccountProviderInterfacePtr m_accountProvider;

        FileGroupInterfacePtr m_fileGroup;
        ArchivatorInterfacePtr m_archivator;

        typedef Hashtable<ConstString, AccountPtr> HashtableAccounts;
        HashtableAccounts m_accounts;

        FactoryInterfacePtr m_factoryAccounts;

        ConstString m_currentAccountId;
        ConstString m_globalAccountId;
        ConstString m_defaultAccountId;

        uint32_t m_playerEnumerator;

        bool m_invalidateAccounts;
    };
}