#	pragma once

#	include "ResourceManager.h"

#	include "InputHandler.h"
#	include "Eventable.h"
#	include "math/vec3.h"
#	include "Account.h"
#	include "TextField.h"

#	include <list>
#	include <map>

extern "C" 
{ 
	struct _object; 
	typedef _object PyObject;
}

class XmlElement;

namespace Menge
{
	class AccountManager
	{
	public:
		AccountManager();
		~AccountManager();
	public:
		void initialize(PyObject * _pyPersonality);
		void finalize();
	public:
		void loadAccounts();
		void loaderAccounts_( XmlElement* _xml );
	public:
		void createAccount( const String& _accountName );
		void deleteAccount( const String& _accountName );
		void selectAccount( const String& _accountName );
		void saveAccount( const String& _accountName );
		void saveAccounts();

		Account * getCurrentAccount();

		void saveAccountsInfo();

	private:
		TStringVector m_loadedAccounts;
		typedef std::map<String, Account*> TAccountMap;
		TAccountMap m_accounts;

		bool m_loadingAccounts;
		String m_defaultAccountName;
		Account * m_currentAccount;
		PyObject * m_pyPersonality;
	};
}