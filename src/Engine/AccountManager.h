#pragma once

#include "Interface/AccountInterface.h"

#include "Core/ServiceBase.h"

#include "Factory/Factory.h"

#include "pybind/types.hpp"

#include "Config/Map.h"

namespace Mengine
{
	class Account;
    
	class AccountManager
        : public ServiceBase<AccountServiceInterface>
	{
	public:
		AccountManager();
		~AccountManager() override;
        
    public:
        bool _initializeService() override;
        void _finalizeService() override;

	public:
		void setAccountProviderInterface(const AccountProviderInterfacePtr & _accountProvider ) override;

	public:
		AccountInterfacePtr createAccount() override;
		AccountInterfacePtr createGlobalAccount() override;

	public:
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
		AccountInterfacePtr getAccount( const ConstString& _accountID ) override;

		void visitAccounts( AccountVisitorInterface * _visitor ) const override;

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
				
		typedef Map<ConstString, AccountInterfacePtr> TMapAccounts;
		TMapAccounts m_accounts;
		
		FactoryPtr m_factoryAccounts;

        ConstString m_currentAccountID;
        ConstString m_globalAccountID;
        ConstString m_defaultAccountID;

		uint32_t m_playerEnumerator;
	};
}