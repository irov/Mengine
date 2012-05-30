#	pragma once

#	include "Config/Typedef.h"
#	include "Core/Holder.h"

#	include "pybind/types.hpp"

#	include <map>

namespace Menge
{
	class Account;

	class AccountManagerListener
	{
	public:
		virtual void onCreateAccount( const WString & _accountID ) = 0;
	};

	typedef std::map<WString, Account *> TMapAccounts;

	class AccountManager
		: public Holder<AccountManager>
	{
	public:
		AccountManager( const WString & _accountsPath, AccountManagerListener * _listener );
		~AccountManager();

	public:
		WString createAccount();

	public:
		void addSetting( const WString& _setting, const WString& _defaultValue, PyObject* _applyFunc );
		void changeSetting( const WString & _setting, const WString & _value );

	public:
		void deleteAccount( const WString& _accountID );
		bool selectAccount( const WString& _accountID );
		void saveAccount( const WString& _accountID );

	public:
		void saveAccounts();

	public:
		bool loadAccountsInfo();
		void saveAccountsInfo();

	public:
		void setDefaultAccount( const WString & _accountID );
		const WString & getDefaultAccount() const;

		bool hasDefaultAccount() const;

		bool selectDefaultAccount();

	public:
		bool hasCurrentAccount() const;
		Account * getCurrentAccount();
		Account * getAccount( const WString& _accountID );

		const TMapAccounts & getAccounts() const;

	public:
		

	protected:
		Account* loadAccount_( const WString& _accountID );
		void createAccount_( const WString& _accountID );

	protected:
		WString m_accountsPath;

		AccountManagerListener * m_listener;		
		
		TMapAccounts m_accounts;

		WString m_defaultAccountID;

		Account * m_currentAccount;

		size_t m_playerEnumerator;
	};
}