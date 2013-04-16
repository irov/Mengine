#	pragma once

#   include "Interface/AccountInterface.h"

#	include "Config/Typedef.h"

#	include "pybind/types.hpp"

#	include <map>

namespace Menge
{
	class Account;
    	
	typedef std::map<WString, AccountInterface *> TMapAccounts;

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
        bool initialize( const FilePath & _accountsPath, AccountServiceListener * _listener ) override;
        void finalize() override;

	public:
		AccountInterface * createAccount() override;

	public:
		void addSetting( const WString& _setting, const WString& _defaultValue, PyObject* _applyFunc ) override;
		void changeSetting( const WString & _setting, const WString & _value ) override;

	public:
		void deleteAccount( const WString& _accountID ) override;
		bool selectAccount( const WString& _accountID ) override;
		bool saveAccount( const WString& _accountID ) override;

	public:
		void saveAccounts() override;

	public:
		bool loadAccountsInfo() override;
		bool saveAccountsInfo() override;

	public:
		void setDefaultAccount( const WString & _accountID ) override;
		const WString & getDefaultAccount() const override;

		bool hasDefaultAccount() const override;

		bool selectDefaultAccount() override;

	public:
		bool hasCurrentAccount() const override;
		AccountInterface * getCurrentAccount() override;
		AccountInterface * getAccount( const WString& _accountID ) override;

		const TMapAccounts & getAccounts() const override;

	protected:
		Account * loadAccount_( const WString& _accountID );
		Account * createAccount_( const WString& _accountID );

    protected:
        Account * newAccount_( const WString& _accountID );

    protected:
        void unselectCurrentAccount_();

	protected:
        FilePath m_accountsPath;
        AccountServiceListener * m_accountListener;

        ServiceProviderInterface * m_serviceProvider;
		
		TMapAccounts m_accounts;

		WString m_defaultAccountID;

		AccountInterface * m_currentAccount;

		size_t m_playerEnumerator;
	};
}