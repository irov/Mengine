#	include "AccountManager.h"
#	include "Account.h"

#	include "Core/String.h"
#	include "Core/File.h"
#	include "LogEngine.h"

#	include "ConfigFile.h"

#	include "Consts.h"

#	include "BinParser.h"

#	include "LoaderEngine.h"
#	include "FileEngine.h"
#	include "ScriptEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	AccountManager::AccountManager( AccountManagerListener * _listener )
		: m_listener(_listener)
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
	String AccountManager::createAccount()
	{
		String accountID = "Player_";
		accountID += Utils::toString( m_playerEnumerator );
		
		createAccount_( accountID );
		
		++m_playerEnumerator;
		
		return accountID;
	}
	//////////////////////////////////////////////////////////////////////////
	void AccountManager::addSetting( const String& _setting, const String& _defaultValue, PyObject* _applyFunc )
	{
		if( m_currentAccount == 0 )
		{
			return;
		}

		m_currentAccount->addSetting( _setting, _defaultValue, _applyFunc );		
	}
	//////////////////////////////////////////////////////////////////////////
	void AccountManager::changeSetting( const String & _setting, const String & _value )
	{
		if( m_currentAccount == 0 )
		{
			return;
		}

		m_currentAccount->changeSetting( _setting, _value );
	}
	//////////////////////////////////////////////////////////////////////////
	void AccountManager::createAccount_( const String& _accountID )
	{
		TMapAccounts::iterator it_find = m_accounts.find( _accountID );
		if( it_find != m_accounts.end() )
		{
			MENGE_LOG_ERROR( "Warning: Account with ID '%s' already exist. Account not created"
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

		const ConstString & folder = newAccount->getFolder();

		FileEngine::get()
			->createDirectory( Consts::get()->c_user, Helper::to_str(folder) );

		newAccount->save();
		saveAccountsInfo();
	}
	//////////////////////////////////////////////////////////////////////////
	void AccountManager::deleteAccount( const String& _accountID )
	{
		TMapAccounts::iterator it_find = m_accounts.find( _accountID );

		if( it_find == m_accounts.end() )
		{
			MENGE_LOG_ERROR( "Can't delete account '%s'. There is no account with such ID"
				, _accountID.c_str() 
				);

			return;
		}

		if( m_currentAccount )
		{
			const ConstString & folder = m_currentAccount->getFolder();
				
			if( Helper::to_str(folder) == _accountID )
			{
				m_currentAccount = 0;
			}
		}

		const ConstString & folder = it_find->second->getFolder();

		FileEngine::get()->
			removeDirectory( Consts::get()->c_user, Helper::to_str(folder) );

		delete it_find->second;

		m_accounts.erase( it_find );

		saveAccountsInfo();
	}
	//////////////////////////////////////////////////////////////////////////
	void AccountManager::selectAccount( const String& _accountID )
	{
		TMapAccounts::iterator it_find = m_accounts.find( _accountID );

		if( it_find == m_accounts.end() )
		{
			MENGE_LOG_ERROR( "Can't select account '%s'. There is no account with such ID"
				, _accountID.c_str() 
				);
			
			return;
		}

		m_currentAccount = it_find->second;
		m_currentAccount->load();
		m_currentAccount->apply();

		saveAccountsInfo();
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
	Account* AccountManager::getAccount( const String& _accountID )
	{
		TMapAccounts::iterator it_find = m_accounts.find( _accountID );
		
		if( it_find == m_accounts.end() )
		{
			MENGE_LOG_ERROR( "Account with ID '%s' not found"
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
	void AccountManager::setDefaultAccount( const String & _accountID )
	{
		m_defaultAccountID = _accountID;

		this->saveAccountsInfo();
	}
	//////////////////////////////////////////////////////////////////////////
	const String & AccountManager::getDefaultAccount() const
	{
		return m_defaultAccountID;
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
	Account* AccountManager::loadAccount_( const String& _accountID )
	{
		Account* account = new Account( _accountID );

		m_currentAccount = account;

		m_listener->onCreateAccount( _accountID );

		account->load();

		return account;
	}
	//////////////////////////////////////////////////////////////////////////
	bool AccountManager::loadAccounts( const String & _accFilename )
	{
		//String accFilename = "Accounts.xml";

		bool accountsExist = FileEngine::get()
			->existFile( Consts::get()->c_user, _accFilename );

		if( accountsExist == false )
		{
			return false;
		}

		//if( loaderAccounts_( accFilename ) == false )

		ConfigFile config;
		
		if( config.load( Consts::get()->c_user, _accFilename ) == false )
		{
			MENGE_LOG_ERROR( "Parsing Accounts failed 'Accounts.ini'"
				);

			return false;
		}
		

		unsigned int playerCount;

		config.getSettingUInt( "AccountCount", "SETTINGS", playerCount );
		config.getSettingUInt( "AccountEnumerator", "SETTINGS", m_playerEnumerator );
		config.getSetting( "DefaultAccountID", "SETTINGS", m_defaultAccountID );

		String selectAccountID;
		config.getSetting( "SelectAccountID", "SETTINGS", selectAccountID );

		for( unsigned int
			it = 0, 
			it_end = playerCount;
		it != it_end;
		++it )
		{
			String accountID;
			config.indexSetting( it, "Account", "ACCOUNTS", accountID );

			Account * account = this->loadAccount_( accountID );

			m_accounts.insert( std::make_pair( accountID, account ) );
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
		FileOutputInterface* file = FileEngine::get()
			->openOutputFile( Consts::get()->c_user, "Accounts.ini" );

		if( file == 0 )
		{
			MENGE_LOG_ERROR( "can't open file for writing. Accounts 'Accounts.ini' settings not saved"
				);

			return;
		}

		Utils::stringWrite( file, "[SETTINGS]\n" );

		if( m_defaultAccountID.empty() == false )
		{
			Utils::stringWrite( file, "DefaultAccountID = " + m_defaultAccountID + "\n" );
		}

		if( m_currentAccount )
		{
			const ConstString & folder = m_currentAccount->getFolder();
			Utils::stringWrite( file, "SelectAccountID = " + Helper::to_str(folder) + "\n" );
		}

		Utils::stringWrite( file, "AccountCount = " + Utils::toString( ( unsigned int )m_accounts.size() ) + "\n" );
		Utils::stringWrite( file, "AccountEnumerator = " + Utils::toString( m_playerEnumerator ) + "\n" );
		

		Utils::stringWrite( file, "[ACCOUNTS]\n" );

		for( TMapAccounts::iterator 
			it = m_accounts.begin(), 
			it_end = m_accounts.end();
		it != it_end;
		++it )
		{
			Utils::stringWrite( file, "Account = " + it->first + "\n" );
		}

		file->close();
	}
	//////////////////////////////////////////////////////////////////////////
	void AccountManager::saveAccount( const String& _accountID )
	{
		TMapAccounts::iterator it_find = m_accounts.find( _accountID );

		if( it_find == m_accounts.end() )
		{
			MENGE_LOG_ERROR( "Warning: Account with ID '%s' does not exist. Can't save"
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