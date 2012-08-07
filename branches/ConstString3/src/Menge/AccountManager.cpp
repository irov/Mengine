#	include "AccountManager.h"
#	include "Account.h"

#	include "Core/String.h"
#	include "Core/File.h"

#	include "Config/Typedef.h"

#	include "LogEngine.h"

#	include "ConfigFile/ConfigFile.h"

#	include "Consts.h"

#	include "FileEngine.h"
#	include "ScriptEngine.h"

#	include "boost/format.hpp"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	AccountManager::AccountManager( const WString & _accountsPath, AccountManagerListener * _listener )
		: m_accountsPath(_accountsPath)
		, m_listener(_listener)
		, m_currentAccount(0)
		, m_playerEnumerator(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	AccountManager::~AccountManager()
	{
		for( TMapAccounts::iterator 
			it = m_accounts.begin(), 
			it_end = m_accounts.end();
		it != it_end;
		++it )
		{
			delete it->second;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	WString AccountManager::createAccount()
	{
		WString accountID = (boost::wformat(L"Player_%d") % m_playerEnumerator).str();

		this->createAccount_( accountID );
		
		++m_playerEnumerator;
		
		return accountID;
	}
	//////////////////////////////////////////////////////////////////////////
	void AccountManager::addSetting( const WString& _setting, const WString& _defaultValue, PyObject* _applyFunc )
	{
		if( m_currentAccount == 0 )
		{
			return;
		}

		m_currentAccount->addSetting( _setting, _defaultValue, _applyFunc );		
	}
	//////////////////////////////////////////////////////////////////////////
	void AccountManager::changeSetting( const WString & _setting, const WString & _value )
	{
		if( m_currentAccount == 0 )
		{
			MENGE_LOG_ERROR("AccountManager::changeSetting not setup currentAccount [%S, %S]"
				, _setting.c_str()
				, _value.c_str()
				);

			return;
		}

		m_currentAccount->changeSetting( _setting, _value );
	}
	//////////////////////////////////////////////////////////////////////////
	void AccountManager::createAccount_( const WString& _accountID )
	{
		TMapAccounts::iterator it_find = m_accounts.find( _accountID );
		if( it_find != m_accounts.end() )
		{
			MENGE_LOG_ERROR( "AccountManager::createAccount_: Account with ID '%S' already exist. Account not created"
				, _accountID.c_str() 
				);

			return;
		}
				
		Account* newAccount = new Account( _accountID );

		m_accounts.insert( std::make_pair( _accountID, newAccount ) );

		m_currentAccount = newAccount;

		m_listener->onCreateAccount( _accountID );
		//if( ScriptEngine::get()
		//	->hasModuleFunction( m_pyPersonality, ("onCreateAccount") ) == false )
		//{
		//	//PyObject* uName = PyUnicode_DecodeUTF8( _accountName.c_str(), _accountName.length(), NULL );
		//	ScriptEngine::get()
		//		->callModuleFunction( m_pyPersonality, ("onCreateAccount"), "(s)", _accountID.c_str() );

		//	//String accountNameAnsi = Holder<Application>::get()->utf8ToAnsi( _accountName );
		//	//Holder<ScriptEngine>::get()
		//	//	->callModuleFunction( m_pyPersonality, ("onCreateAccount"), "(s)", accountNameAnsi.c_str() );
		//}
		//else
		//{
		//	MENGE_LOG_ERROR( "Warning: Personality module has no method 'onCreateAccount'. Ambigous using accounts" );
		//}

		const WString & folder = newAccount->getName();

		FileEngine::get()
			->createDirectory( Consts::get()->c_user, folder );

		newAccount->save();

		this->saveAccountsInfo();
	}
	//////////////////////////////////////////////////////////////////////////
	void AccountManager::deleteAccount( const WString& _accountID )
	{
		TMapAccounts::iterator it_find = m_accounts.find( _accountID );

		if( it_find == m_accounts.end() )
		{
			MENGE_LOG_ERROR( "AccountManager::deleteAccount Can't delete account '%S'. There is no account with such ID"
				, _accountID.c_str() 
				);

			return;
		}

		if( m_currentAccount )
		{
			const WString & name = m_currentAccount->getName();
				
			if( name == _accountID )
			{
				m_currentAccount = 0;
			}
		}

		const WString & folder = it_find->second->getName();

		FileEngine::get()->
			removeDirectory( Consts::get()->c_user, folder );

		delete it_find->second;

		m_accounts.erase( it_find );

		this->saveAccountsInfo();
	}
	//////////////////////////////////////////////////////////////////////////
	bool AccountManager::selectAccount( const WString& _accountID )
	{
		TMapAccounts::iterator it_find = m_accounts.find( _accountID );

		if( it_find == m_accounts.end() )
		{
			MENGE_LOG_ERROR( "AccountManager::selectAccount Can't select account '%S'. There is no account with such ID"
				, _accountID.c_str() 
				);
			
			return false;
		}

		m_currentAccount = it_find->second;
		m_currentAccount->load();
		m_currentAccount->apply();

		saveAccountsInfo();

		m_listener->onSelectAccount( _accountID );

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool AccountManager::hasCurrentAccount() const
	{
		return m_currentAccount != 0;
	}
	//////////////////////////////////////////////////////////////////////////
	Account* AccountManager::getCurrentAccount()
	{
		return m_currentAccount;
	}
	//////////////////////////////////////////////////////////////////////////
	Account* AccountManager::getAccount( const WString& _accountID )
	{
		TMapAccounts::iterator it_find = m_accounts.find( _accountID );
		
		if( it_find == m_accounts.end() )
		{
			MENGE_LOG_ERROR( "Account with ID '%S' not found"
				, _accountID.c_str() 
				);

			return 0;
		}

		return it_find->second;
	}
	//////////////////////////////////////////////////////////////////////////
	const TMapAccounts & AccountManager::getAccounts() const
	{
		return m_accounts;
	}
	//////////////////////////////////////////////////////////////////////////
	void AccountManager::setDefaultAccount( const WString & _accountID )
	{
		m_defaultAccountID = _accountID;

		this->saveAccountsInfo();
	}
	//////////////////////////////////////////////////////////////////////////
	const WString & AccountManager::getDefaultAccount() const
	{
		return m_defaultAccountID;
	}
	//////////////////////////////////////////////////////////////////////////
	bool AccountManager::hasDefaultAccount() const
	{
		bool result = m_defaultAccountID.empty() == false;

		return result;
	}
	//////////////////////////////////////////////////////////////////////////
	bool AccountManager::selectDefaultAccount()
	{
		if( m_defaultAccountID.empty() == true )
		{
			return false;
		}

		this->selectAccount( m_defaultAccountID );
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	Account* AccountManager::loadAccount_( const WString& _accountID )
	{
		Account* account = new Account( _accountID );

		m_currentAccount = account;

		m_listener->onCreateAccount( _accountID );

		account->load();

		return account;
	}
	//////////////////////////////////////////////////////////////////////////
	bool AccountManager::loadAccountsInfo()
	{
		//String accFilename = "Accounts.xml";

		bool accountsExist = FileEngine::get()
			->existFile( Consts::get()->c_user, m_accountsPath );

		if( accountsExist == false )
		{
			MENGE_LOG_ERROR( "AccountManager::loadAccounts not exist accounts '%S'"
				, m_accountsPath.c_str()
				);

			return false;
		}

		//if( loaderAccounts_( accFilename ) == false )

		ConfigFile config;

		FileInputStreamInterface* file = FileEngine::get()
			->openInputFile( Consts::get()->c_user, m_accountsPath );
		
		if( config.load( file ) == false )
		{
			MENGE_LOG_ERROR( "AccountManager::loadAccounts parsing accounts failed '%S'"
				, m_accountsPath.c_str()
				);

			return false;
		}

		file->close();		

		//unsigned int playerCount;

		//config.getSettingUInt( L"SETTINGS", L"AccountCount", playerCount );
		config.getSettingUInt( L"SETTINGS", L"AccountEnumerator", m_playerEnumerator );
		config.getSetting( L"SETTINGS", L"DefaultAccountID", m_defaultAccountID );

		WString selectAccountID;
		config.getSetting( L"SETTINGS", L"SelectAccountID", selectAccountID );

		TVectorWString values;
		config.getSettings( L"ACCOUNTS", L"Account", values );

		for( TVectorWString::const_iterator
			it = values.begin(), 
			it_end = values.end();
		it != it_end;
		++it )
		{
			const WString & name = *it;
			//config.indexSetting( it, "ACCOUNTS", "Account", accountID );

			Account * account = this->loadAccount_( name );

			m_accounts.insert( std::make_pair( name, account ) );
		}

		if( selectAccountID.empty() == false )
		{
			selectAccount( selectAccountID );
		}
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void AccountManager::saveAccountsInfo()
	{
		FileOutputStreamInterface* file = FileEngine::get()
			->openOutputFile( Consts::get()->c_user, m_accountsPath );

		if( file == 0 )
		{
			MENGE_LOG_ERROR( "AccountManager::saveAccountsInfo can't open file for writing. Accounts '%S' settings not saved"
				, m_accountsPath.c_str()
				);

			return;
		}

		ConfigFile config;

		//Utils::stringWriteU( file, L"[SETTINGS]\n" );

		if( m_defaultAccountID.empty() == false )
		{
			config.setSetting( L"SETTINGS", L"DefaultAccountID", m_defaultAccountID );
			//Utils::stringWriteU( file, L"DefaultAccountID = " + m_defaultAccountID + L"\n" );
		}

		if( m_currentAccount )
		{
			const WString & name = m_currentAccount->getName();

			config.setSetting( L"SETTINGS", L"SelectAccountID", name );
			//Utils::stringWriteU( file, L"SelectAccountID = " + name + L"\n" );
		}

		//Utils::stringWriteU( file, L"AccountCount = " + Utils::toString( ( unsigned int )m_accounts.size() ) + "\n" );

		config.setSetting( L"SETTINGS", L"AccountEnumerator", Utils::toWString( m_playerEnumerator ) );
		//Utils::stringWriteU( file, L"AccountEnumerator = " + Utils::toWString( m_playerEnumerator ) + L"\n" );
		
		//Utils::stringWriteU( file, L"[ACCOUNTS]\n" );

		for( TMapAccounts::iterator 
			it = m_accounts.begin(), 
			it_end = m_accounts.end();
		it != it_end;
		++it )
		{
			config.setSetting( L"ACCOUNTS", L"Account", it->first );
			//Utils::stringWriteU( file, L"Account = " + it->first + L"\n" );
		}

		config.save( file );

		file->close();
	}
	//////////////////////////////////////////////////////////////////////////
	void AccountManager::saveAccount( const WString& _accountID )
	{
		TMapAccounts::iterator it_find = m_accounts.find( _accountID );

		if( it_find == m_accounts.end() )
		{
			MENGE_LOG_ERROR( "Warning: Account with ID '%S' does not exist. Can't save"
				, _accountID.c_str() 
				);

			return;
		}
	
		it_find->second->save();
	}
	//////////////////////////////////////////////////////////////////////////
	void AccountManager::saveAccounts()
	{
		for( TMapAccounts::iterator 
			it = m_accounts.begin(), 
			it_end = m_accounts.end();
		it != it_end;
		++it )
		{
			it->second->save();
		}
	}
}