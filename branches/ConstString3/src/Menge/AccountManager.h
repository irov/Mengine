#	pragma once

#	include "Config/Typedef.h"
#	include "Core/Holder.h"

#	include "Loadable.h"

#	include <map>

namespace Menge
{
	class Account;

	class AccountManagerListener
	{
	public:
		virtual void onCreateAccount( const String & accountId ) = 0;
	};

	class AccountManager
		: public Holder<AccountManager>
		, public Loadable
	{
	public:
		AccountManager( AccountManagerListener * _listener );
		~AccountManager();

	public:
		String createNewAccount();

	public:
		void changeSetting( const String & _setting, const String & _value );

	public:
		void deleteAccount( const String& _accountID );
		void selectAccount( const String& _accountID );
		void saveAccount( const String& _accountID );
		void saveAccounts();
		void saveAccountsInfo();

	public:
		Account * getCurrentAccount();
		Account * getAccount( const String& _accountID );

	public:
		bool loadAccounts( const String & _accFilename );

	protected:
		void loader( BinParser * _parser ) override;
		void loaded() override;

	protected:
		Account* loadAccount_( const String& _accountID );
		void createAccount_( const String& _accountID );

	protected:
		AccountManagerListener * m_listener;

		typedef std::map<String, Account *> TMapAccounts;
		TMapAccounts m_accounts;

		String m_defaultAccountID;
		Account* m_currentAccount;

		int m_playerNumberCounter;
	};
}