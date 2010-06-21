#	include "AccountManager.h"
#	include "Account.h"

#	include "Core/String.h"
#	include "Core/File.h"
#	include "Logger/Logger.h"

#	include "Consts.h"

#	include "XmlEngine.h"
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
		//if( ScriptEngine::hostage()
		//	->hasModuleFunction( m_pyPersonality, ("onCreateAccount") ) == false )
		//{
		//	//PyObject* uName = PyUnicode_DecodeUTF8( _accountName.c_str(), _accountName.length(), NULL );
		//	ScriptEngine::hostage()
		//		->callModuleFunction( m_pyPersonality, ("onCreateAccount"), "(s)", _accountID.c_str() );

		//	//String accountNameAnsi = Holder<Application>::hostage()->utf8ToAnsi( _accountName );
		//	//Holder<ScriptEngine>::hostage()
		//	//	->callModuleFunction( m_pyPersonality, ("onCreateAccount"), "(s)", accountNameAnsi.c_str() );
		//}
		//else
		//{
		//	MENGE_LOG_ERROR( "Warning: Personality module has no method 'onCreateAccount'. Ambigous using accounts" );
		//}

		const String & folder = newAccount->getFolder();

		FileEngine::hostage()
			->createDirectory( Consts::c_user, folder );

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

			FileEngine::hostage()->
				removeDirectory( Consts::c_user, it_find->second->getFolder() );

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
	void AccountManager::loader( XmlElement* _xml )
	{
		XML_SWITCH_NODE( _xml )
		{
			XML_CASE_NODE( "AccountID" )
			{
				String accID;
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "Value", accID );
				}
				//m_accountIDs.push_back( accID );
				m_accounts.insert( std::make_pair<String, Account*>( accID, NULL ) );
			}
			XML_CASE_ATTRIBUTE_NODE( "DefaultAccountID", "Value", m_defaultAccountID );
			XML_CASE_ATTRIBUTE_NODE( "PlayerCounter", "Value", m_playerNumberCounter );
		}
	}
	//////////////////////////////////////////////////////////////////////////
	Account* AccountManager::loadAccount_( const String& _accountID )
	{
		Account* newAccount = new Account( _accountID );

		m_currentAccount = newAccount;

		m_listener->onCreateAccount( _accountID );

		//if( ScriptEngine::hostage()
		//	->hasModuleFunction( m_pyPersonality, ("onCreateAccount") ) )
		//{
		//	ScriptEngine::hostage()
		//		->callModuleFunction( m_pyPersonality, ("onCreateAccount"), "(s)", _accountID.c_str() );

		//}

		newAccount->load();

		return newAccount;
	}
	//////////////////////////////////////////////////////////////////////////
	void AccountManager::loadAccounts()
	{
		String accFilename = "Accounts.xml";

		bool accountsExist = FileEngine::hostage()
			->existFile( Consts::c_user, accFilename );

		if( accountsExist == true )
		{
			//if( loaderAccounts_( accFilename ) == false )
			if( XmlEngine::hostage()
				->parseXmlFileM( Consts::c_user, accFilename, this, &AccountManager::loader ) == false )
			{
				MENGE_LOG_ERROR( "Parsing Accounts ini failed '%s'"
					, accFilename.c_str()
					);

				return;
			}

			for( TMapAccounts::iterator 
				it = m_accounts.begin(), 
				it_end = m_accounts.end();
			it != it_end;
			++it )
			{
				//createAccount( it->name, it->folder );
				it->second = loadAccount_( it->first );
			}

			if( m_defaultAccountID != "" )
			{
				selectAccount( m_defaultAccountID );
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void AccountManager::saveAccountsInfo()
	{
		FileOutputInterface* outFile = FileEngine::hostage()
			->openOutputFile( Consts::c_user, "Accounts.xml" );

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
			Utils::fileWrite( outFile, "\t<DefaultAccountID Value = \"" + m_currentAccount->getFolder() + "\"/>\n" );
		}

		Utils::fileWrite( outFile, "\t<PlayerCounter Value = \"" + Utils::toString( m_playerNumberCounter ) + "\"/>\n" );
		Utils::fileWrite( outFile, "</Accounts>" );

		FileEngine::hostage()
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