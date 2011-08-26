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
		virtual void onCreateAccount( const String & _accountID ) = 0;
	};

	typedef std::map<String, Account *> TMapAccounts;

	class AccountManager
		: public Holder<AccountManager>
	{
	public:
		AccountManager( AccountManagerListener * _listener );
		~AccountManager();

	public:
		String createAccount();

	public:
		void addSetting( const String& _setting, const String& _defaultValue, PyObject* _applyFunc );
		void changeSetting( const String & _setting, const String & _value );

	public:
		void deleteAccount( const String& _accountID );
		void selectAccount( const String& _accountID );
		void saveAccount( const String& _accountID );
		void saveAccounts();
		void saveAccountsInfo();

	public:
		void setDefaultAccount( const String & _accountID );
		const String & getDefaultAccount() const;

		bool selectDefaultAccount();

	public:
		bool hasCurrentAccount() const;
		Account * getCurrentAccount();
		Account * getAccount( const String& _accountID );

		const TMapAccounts & getAccounts() const;

	public:
		bool loadAccounts( const String & _accFilename );

	protected:
		Account* loadAccount_( const String& _accountID );
		void createAccount_( const String& _accountID );

	protected:
		AccountManagerListener * m_listener;

		
		TMapAccounts m_accounts;

		String m_defaultAccountID;

		Account* m_currentAccount;

		unsigned int m_playerEnumerator;
	};
}