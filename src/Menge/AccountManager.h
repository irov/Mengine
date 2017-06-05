#	pragma once

#   include "Interface/AccountInterface.h"

#   include "Core/ServiceBase.h"

#	include "Factory/Factory.h"

#	include "pybind/types.hpp"

#	include "stdex/stl_map.h"

namespace Menge
{
	class Account;
    
	class AccountManager
        : public ServiceBase<AccountServiceInterface>
	{
	public:
		AccountManager();
		~AccountManager();
        
    public:
        bool _initialize() override;
        void _finalize() override;

	public:
		void setAccountProviderInterface(const AccountProviderInterfacePtr & _accountProvider ) override;

	public:
		AccountInterfacePtr createAccount() override;

	public:
		void deleteAccount( const WString& _accountID ) override;
		bool selectAccount( const WString& _accountID ) override;

	public:
		bool loadAccounts() override;
        bool saveAccounts() override;

	public:
		void setDefaultAccount( const WString & _accountID ) override;
		const WString & getDefaultAccount() const override;

		bool isCurrentDefaultAccount() const override;

		bool hasDefaultAccount() const override;

		bool selectDefaultAccount() override;

	public:
		void setGlobalAccount( const WString & _accountID ) override;
		const WString & getGlobalAccount() const override;

		bool hasGlobalAccount() const override;

	public:
		bool hasCurrentAccount() const override;
		const AccountInterfacePtr & getCurrentAccount() override;
		AccountInterfacePtr getAccount( const WString& _accountID ) override;

		void visitAccounts( AccountVisitorInterface * _visitor ) const override;

	protected:
		AccountInterfacePtr loadAccount_( const WString& _accountID );
		AccountInterfacePtr createAccount_( const WString& _accountID );

    protected:
        AccountInterfacePtr newAccount_( const WString& _accountID );

    protected:
        void unselectCurrentAccount_();

	protected:
		AccountProviderInterfacePtr m_accountProvider;
				
		typedef stdex::map<WString, AccountInterfacePtr> TMapAccounts;
		TMapAccounts m_accounts;
		
		FactoryPtr m_factoryAccounts;

		WString m_globalAccountID;
		WString m_defaultAccountID;

		AccountInterfacePtr m_currentAccount;

		uint32_t m_playerEnumerator;
	};
}