#	include "AccountManager.h"
#	include "Account.h"

#	include "Core/String.h"
#	include "Core/File.h"
#	include "Logger/Logger.h"

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
		, m_playerNumberCounter( 0 )
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
	String AccountManager::createNewAccount()
	{
		String accountID = "Player_";
		accountID += Utils::toString( m_playerNumberCounter );
		
		createAccount_( accountID );
		
		++m_playerNumberCounter;
		
		return accountID;
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
			->createDirectory( Consts::get()->c_user, folder.str() );

		newAccount->save();
		saveAccountsInfo();
	}
	//////////////////////////////////////////////////////////////////////////
	void AccountManager::deleteAccount( const String& _accountID )
	{
		TMapAccounts::iterator it_find = m_accounts.find( _accountID );

		if( it_find != m_accounts.end() )
		{
			if( m_currentAccount && ( m_currentAccount->getFolder() == _accountID ) )
			{
				m_currentAccount = 0;
			}

			const ConstString & folder = it_find->second->getFolder();

			FileEngine::get()->
				removeDirectory( Consts::get()->c_user, folder.str() );

			delete it_find->second;

			m_accounts.erase( it_find );
		}
		else
		{
			MENGE_LOG_ERROR( "Can't delete account '%s'. There is no account with such ID"
				, _accountID.c_str() 
				);
		}

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
	void AccountManager::loader( BinParser * _parser )
	{
		BIN_SWITCH_ID( _parser )
		{
			BIN_CASE_ATTRIBUTE_METHOD( Protocol::AccountID_Value, &AccountManager::loadAccount_ );
			BIN_CASE_ATTRIBUTE( Protocol::DefaultAccountID_Value, m_defaultAccountID );
			BIN_CASE_ATTRIBUTE( Protocol::PlayerCounter_Value, m_playerNumberCounter );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void AccountManager::loaded()
	{

	}
	//////////////////////////////////////////////////////////////////////////
	Account* AccountManager::loadAccount_( const String& _accountID )
	{
		Account* account = new Account( _accountID );

		m_currentAccount = account;

		m_listener->onCreateAccount( _accountID );

		account->load();

		m_accounts.insert( std::make_pair( _accountID, account ) );

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


		if( LoaderEngine::get()
			->load( Consts::get()->c_user, _accFilename, this ) == false )
		//if( XmlEngine::get()
		//	->parseXmlFileM( Consts::get()->c_user, accFilename, this, &AccountManager::loader ) == false )
		{
			MENGE_LOG_ERROR( "Parsing Accounts ini failed '%s'"
				, _accFilename.c_str()
				);

			return false;
		}

		if( m_defaultAccountID != "" )
		{
			selectAccount( m_defaultAccountID );
		}
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void AccountManager::saveAccountsInfo()
	{
		FileOutputInterface* outFile = FileEngine::get()
			->openOutputFile( Consts::get()->c_user, "Accounts.xml" );

		if( outFile == NULL )
		{
			MENGE_LOG_ERROR( "Accounts info wouldn't be saved. Can't open file for writing" );
			return;
		}

		Utils::fileWrite( outFile, "<Accounts>\n" );

		for( TMapAccounts::iterator 
			it = m_accounts.begin(), 
			it_end = m_accounts.end();
		it != it_end;
		++it )
		{
			Utils::fileWrite( outFile, "\t<AccountID Value = \"" + it->first + "\"/>\n" );
		}

		if( m_currentAccount != 0 )
		{
			Utils::fileWrite( outFile, "\t<DefaultAccountID Value = \"" + m_currentAccount->getFolder().str() + "\"/>\n" );
		}

		Utils::fileWrite( outFile, "\t<PlayerCounter Value = \"" + Utils::toString( m_playerNumberCounter ) + "\"/>\n" );
		Utils::fileWrite( outFile, "</Accounts>" );

		FileEngine::get()
			->closeOutputFile( outFile );
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