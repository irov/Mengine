#	include "AccountManager.h"
#	include "SceneManager.h"

#	include "Scene.h"
#	include "Player.h"
#	include "Arrow.h"
#	include "Amplifier.h"

#	include "MousePickerSystem.h"
#	include "LightSystem.h"

#	include "ScriptEngine.h"
#	include "FileEngine.h"
#	include "RenderEngine.h"
#	include "ResourceManager.h"
#	include "ScheduleManager.h"
#	include "LogEngine.h"
#	include "RenderEngine.h"
#	include "ProfilerEngine.h"

#	include "XmlEngine.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	AccountManager::AccountManager()
		: m_pyPersonality(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	AccountManager::~AccountManager()
	{
	}
	//////////////////////////////////////////////////////////////////////////
	void AccountManager::initialize(PyObject * _pyPersonality)
	{
		m_pyPersonality = _pyPersonality;
	}
	//////////////////////////////////////////////////////////////////////////
	void AccountManager::finalize()
	{
		for( TAccountMap::iterator it = m_accounts.begin(), it_end = m_accounts.end();
			it != it_end;
			it++ )
		{
			delete it->second;
		}

		m_accounts.clear();
	}
	//////////////////////////////////////////////////////////////////////////
	void AccountManager::createAccount( const String& _accountName )
	{
		TAccountMap::iterator it = m_accounts.find( _accountName );
		if( it != m_accounts.end() )
		{
			MENGE_LOG("Warning: Account with name '%s' already exist. Account not created",
				_accountName.c_str() );
			return;
		}

		Holder<FileEngine>::hostage()->
			createFolder( Holder<FileEngine>::hostage()->getAppDataPath() + "\\" + _accountName );

		Account* newAccount = new Account( _accountName );
		m_accounts.insert( std::make_pair( _accountName, newAccount ) );

		m_currentAccount = newAccount;

		if( Holder<ScriptEngine>::hostage()
			->hasModuleFunction( m_pyPersonality, "onCreateAccount" ) )
		{
			Holder<ScriptEngine>::hostage()
				->callModuleFunction( m_pyPersonality, "onCreateAccount", "(s)", _accountName.c_str() );
		}
		else
		{
			MENGE_LOG("Warning: Personality module has no method 'onCreateAccount'. Ambiguous using accounts" );
		}

		if( m_loadingAccounts == false )
		{
			newAccount->save();
			saveAccountsInfo();
		}
		//m_currentAccount->apply();
	}
	//////////////////////////////////////////////////////////////////////////
	void AccountManager::deleteAccount( const String& _accountName )
	{
		TAccountMap::iterator it_find = m_accounts.find( _accountName );

		if( it_find != m_accounts.end() )
		{
			if( m_currentAccount && ( m_currentAccount->getName() == _accountName ) )
			{
				m_currentAccount = 0;
			}

			Holder<FileEngine>::hostage()->
				deleteFolder( Holder<FileEngine>::hostage()->getAppDataPath() + "\\" + _accountName );

			delete it_find->second;

			m_accounts.erase( it_find );
		}
		else
		{
			MENGE_LOG("Error: Can't delete account '%s'. There is no account with such name"
				, _accountName.c_str() 
				);
		}

		saveAccountsInfo();
	}
	//////////////////////////////////////////////////////////////////////////
	void AccountManager::selectAccount( const String& _accountName )
	{
		TAccountMap::iterator it_find = m_accounts.find( _accountName );

		if( it_find != m_accounts.end() )
		{
			m_currentAccount = it_find->second;
			m_currentAccount->load();
			m_currentAccount->apply();
		}
		else
		{
			MENGE_LOG("Error: Can't select account '%s'. There is no account with such name"
				, _accountName.c_str() 
				);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	Account* AccountManager::getCurrentAccount()
	{
		return m_currentAccount;
	}
	//////////////////////////////////////////////////////////////////////////
	void AccountManager::loaderAccounts_( XmlElement* _xml )
	{
		XML_SWITCH_NODE( _xml )
		{
			String accountName;

			XML_CASE_NODE( "Account" )
			{
				XML_FOR_EACH_ATTRIBUTES()
				{
					XML_CASE_ATTRIBUTE( "Name", accountName );
				}
				m_accounts.insert( std::make_pair( accountName, static_cast<Account*>(0) ) );
			}
			XML_CASE_ATTRIBUTE_NODE( "DefaultAccount", "Name", m_defaultAccountName );
		}		
	}
	//////////////////////////////////////////////////////////////////////////
	void AccountManager::loadAccounts()
	{
		m_loadingAccounts = true;

		String file = Holder<FileEngine>::hostage()->getAppDataPath() + "\\" + "Accounts.ini";

		if( Holder<FileEngine>::hostage()->existFile( file ) )
		{
			if( Holder<XmlEngine>::hostage()
				->parseXmlFileM( file, this, &AccountManager::loaderAccounts_ ) == false )
			{
				MENGE_LOG("Parsing Accounts xml failed '%s'\n"
					, file.c_str()
					);
				return;
			}

			TStringVector accountNames;

			for( TAccountMap::iterator it = m_accounts.begin(), it_end = m_accounts.end();
				it != it_end;
				it++ )
			{
				accountNames.push_back( it->first );
			}

			m_accounts.clear();

			for( TStringVector::iterator it = accountNames.begin(), it_end = accountNames.end();
				it != it_end;
				it++ )
			{
				createAccount( (*it) );
			}

			if( m_defaultAccountName != "" )
			{
				selectAccount( m_defaultAccountName );
			}
		}

		m_loadingAccounts = false;
	}
	//////////////////////////////////////////////////////////////////////////
	void AccountManager::saveAccountsInfo()
	{
		OutStreamInterface* outStream = Holder<FileEngine>::hostage()
			->openOutStream( "Accounts.ini", false );

		outStream->write( "<Accounts>\n" );

		for( TAccountMap::iterator it = m_accounts.begin(), it_end = m_accounts.end();
			it != it_end;
			it++ )
		{
			outStream->write( "\t<Account Name = \"" + it->first + "\"/>\n" );
		}

		if( m_currentAccount != 0 )
		{
			outStream->write( "\t<DefaultAccount Name = \"" + m_currentAccount->getName() + "\"/>\n" );
		}

		outStream->write( "</Accounts>" );

		Holder<FileEngine>::hostage()->closeOutStream( outStream );

		/*for( TAccountMap::iterator it = m_accounts.begin(), it_end = m_accounts.end();
		it != it_end;
		it++ )
		{
		it->second->save();
		}*/
	}
	//////////////////////////////////////////////////////////////////////////
	void AccountManager::saveAccount( const String& _accountName )
	{
		TAccountMap::iterator it_find = m_accounts.find( _accountName );

		if( it_find != m_accounts.end() )
		{
			it_find->second->save();
		}
		else
		{
			MENGE_LOG("Warning: Account '%s' does not exist. Can't save"
				, _accountName.c_str()
				);
		}
	}
	//////////////////////////////////////////////////////////////////////////
	void AccountManager::saveAccounts()
	{
		for( TAccountMap::iterator it = m_accounts.begin(), it_end = m_accounts.end();
			it != it_end;
			it++ )
		{
			it->second->save();
		}
	}
	//////////////////////////////////////////////////////////////////////////
}