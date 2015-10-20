#	pragma once

#   include "Interface/AccountInterface.h"

#	include "Factory/FactoryStore.h"

#	include "pybind/types.hpp"

#	include "stdex/stl_map.h"

namespace Menge
{
	class Account;
    
	class AccountManager
        : public AccountServiceInterface
	{
	public:
		AccountManager();
		~AccountManager();
        
    public:
        void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override;
        ServiceProviderInterface * getServiceProvider() const override;

    public:
        bool initialize( const FilePath & _accountsPath, uint32_t _projectVersion, AccountProviderInterface * _listener ) override;
        void finalize() override;

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
        FilePath m_accountsPath;
		uint32_t m_projectVersion;

        AccountProviderInterface * m_accountListener;

        ServiceProviderInterface * m_serviceProvider;
		
		typedef stdex::map<WString, AccountInterfacePtr> TMapAccounts;
		TMapAccounts m_accounts;
		
		typedef FactoryPoolStore<Account, 8> TFactoryAccounts;
		TFactoryAccounts m_factoryAccounts;

		WString m_defaultAccountID;

		AccountInterfacePtr m_currentAccount;

		uint32_t m_playerEnumerator;
	};
}