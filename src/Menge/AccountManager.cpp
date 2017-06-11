#	include "AccountManager.h"
#	include "Account.h"

#   include "Interface/ApplicationInterface.h"
#   include "Interface/OptionsInterface.h"
#   include "Interface/UnicodeInterface.h"
#   include "Interface/FileSystemInterface.h"
#   include "Interface/StringizeInterface.h"
#   include "Interface/ConfigInterface.h"

#   include "Factory/FactoryPool.h"

#	include "Logger/Logger.h"

#	include "Consts.h"

#	include "Core/IniUtil.h"

#	include "Core/String.h"

#	include "Config/Typedef.h"
#	include "Config/Stringstream.h"

#	include <stdio.h>
#	include <wchar.h>

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( AccountService, Menge::AccountManager );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	AccountManager::AccountManager()
		: m_accountProvider(nullptr)
		, m_playerEnumerator(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	AccountManager::~AccountManager()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    bool AccountManager::_initialize()
    {
		LOGGER_INFO( m_serviceProvider )("Initializing Account manager..."
			);
		
        m_factoryAccounts = new FactoryPool<Account, 8>( m_serviceProvider );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AccountManager::_finalize()
    {
        LOGGER_WARNING(m_serviceProvider)("AccountManager::finalize save accounts"
            );

        WString lastAccount = m_currentAccountID;
        this->unselectCurrentAccount_();

		m_currentAccountID = lastAccount;

        this->saveAccounts();

		m_currentAccountID.clear();
        m_accounts.clear();

        m_factoryAccounts = nullptr;
    }
	//////////////////////////////////////////////////////////////////////////
	void AccountManager::setAccountProviderInterface(const AccountProviderInterfacePtr & _accountProvider)
	{
		m_accountProvider = _accountProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	AccountInterfacePtr AccountManager::createAccount()
	{
        uint32_t new_playerID = ++m_playerEnumerator;

		WStringstream streamAccountID;
		streamAccountID << L"Player_" << new_playerID;

		WString accountID = streamAccountID.str();
        
		AccountInterfacePtr account = this->createAccount_( accountID );

        if( account == nullptr )
        {
            return nullptr;
        }
        	
		return account;
	}
	//////////////////////////////////////////////////////////////////////////
	AccountInterfacePtr AccountManager::createGlobalAccount()
	{
		uint32_t new_playerID = ++m_playerEnumerator;

		WStringstream streamAccountID;
		streamAccountID << L"Player_" << new_playerID;

		WString accountID = streamAccountID.str();

		AccountInterfacePtr account = this->createGlobalAccount_( accountID );

		if( account == nullptr )
		{
			return nullptr;
		}

		return account;
	}
	//////////////////////////////////////////////////////////////////////////
	AccountInterfacePtr AccountManager::createAccount_( const WString& _accountID )
	{
		TMapAccounts::iterator it_find = m_accounts.find( _accountID );

		if( it_find != m_accounts.end() )
		{
			LOGGER_ERROR(m_serviceProvider)("AccountManager::createAccount_: Account with ID '%ls' already exist. Account not created"
				, _accountID.c_str() 
				);

			return nullptr;
		}

		this->unselectCurrentAccount_();
				
		AccountInterfacePtr newAccount = this->newAccount_( _accountID );

        if( newAccount == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("AccountManager::createAccount_: Account with ID '%ls' invalid create. Account not created"
                , _accountID.c_str() 
                );

            return nullptr;
        }
        
		m_currentAccountID = newAccount->getID();

		m_accounts.insert( std::make_pair( _accountID, newAccount ) );

        if( m_accountProvider != nullptr )
        {
		    m_accountProvider->onCreateAccount( _accountID, false );
        }        	

		newAccount->apply();

		if( m_accountProvider != nullptr )
		{
			m_accountProvider->onSelectAccount( _accountID );
		}

        return newAccount;
	}
	//////////////////////////////////////////////////////////////////////////	
	AccountInterfacePtr AccountManager::createGlobalAccount_( const WString& _accountID )
	{
		TMapAccounts::iterator it_find = m_accounts.find( _accountID );

		if( it_find != m_accounts.end() )
		{
			LOGGER_ERROR( m_serviceProvider )("AccountManager::createGlobalAccount_: Account with ID '%ls' already exist. Account not created"
				, _accountID.c_str()
				);

			return nullptr;
		}
				
		AccountInterfacePtr newAccount = this->newAccount_( _accountID );

		if( newAccount == nullptr )
		{
			LOGGER_ERROR( m_serviceProvider )("AccountManager::createGlobalAccount_: Account with ID '%ls' invalid create. Account not created"
				, _accountID.c_str()
				);

			return nullptr;
		}

		m_globalAccountID = newAccount->getID();

		m_accounts.insert( std::make_pair( _accountID, newAccount ) );
		
		newAccount->apply();

		return newAccount;
	}
    //////////////////////////////////////////////////////////////////////////
    void AccountManager::unselectCurrentAccount_()
    {
        if( m_currentAccountID.empty() == true )
        {
            return;
        }

		WString currentAccount = m_currentAccountID;
        
        if( m_accountProvider != nullptr )
        {
            m_accountProvider->onUnselectAccount( currentAccount );
        }

		if( m_currentAccountID == currentAccount )
		{
			m_currentAccountID.clear();
		}
    }
    //////////////////////////////////////////////////////////////////////////
    AccountInterfacePtr AccountManager::newAccount_( const WString& _accountID )
    {
		WString accountFolder = _accountID;
		accountFolder += '/';

        if( PLATFORM_SERVICE(m_serviceProvider)
            ->existDirectory( accountFolder ) == false )
        {
            if( PLATFORM_SERVICE(m_serviceProvider)
                ->createDirectory( accountFolder ) == false )
            {
                LOGGER_ERROR(m_serviceProvider)("AccountManager::createAccount_: Account '%ls' failed create directory"
                    , accountFolder.c_str()
                    );

                return nullptr;
            }
        }

		String utf8_accountFolder;
		if( Helper::unicodeToUtf8( m_serviceProvider, accountFolder, utf8_accountFolder ) == false )
		{
			LOGGER_ERROR( m_serviceProvider )("AccountManager::newAccount_ can't convert accountID '%ls' to utf8"
				, _accountID.c_str()
				);

			return nullptr;
		}

		FilePath folder = Helper::stringizeFilePath( m_serviceProvider, utf8_accountFolder.c_str(), utf8_accountFolder.size() );
		
        AccountPtr newAccount = m_factoryAccounts->createObject();

        newAccount->setServiceProvider( m_serviceProvider );

		uint32_t projectVersion = APPLICATION_SERVICE( m_serviceProvider )
			->getProjectVersion();
		
		if( newAccount->initialize( _accountID, folder, projectVersion ) == false )
		{
			return nullptr;				 
		}

        return newAccount;
    }
	//////////////////////////////////////////////////////////////////////////
	void AccountManager::deleteAccount( const WString& _accountID )
	{
		TMapAccounts::iterator it_find = m_accounts.find( _accountID );

		if( it_find == m_accounts.end() )
		{
			LOGGER_ERROR(m_serviceProvider)("AccountManager::deleteAccount Can't delete account '%ls'. There is no account with such ID"
				, _accountID.c_str() 
				);

			return;
		}

        AccountInterfacePtr account = it_find->second;

		if( m_currentAccountID.empty() == false )
		{
            if( m_currentAccountID == _accountID )
			{
                this->unselectCurrentAccount_();
			}
		}

		m_accounts.erase( it_find );

        if( m_accountProvider != nullptr )
        {
            m_accountProvider->onDeleteAccount( _accountID );
        }  
	}
	//////////////////////////////////////////////////////////////////////////
	bool AccountManager::selectAccount( const WString& _accountID )
	{
		TMapAccounts::iterator it_find = m_accounts.find( _accountID );

		if( it_find == m_accounts.end() )
		{
			LOGGER_ERROR(m_serviceProvider)("AccountManager::selectAccount Can't select account '%ls'. There is no account with such ID"
				, _accountID.c_str() 
				);
			
			return false;
		}

        if( m_currentAccountID.empty() == false )
        {
            if( m_currentAccountID != _accountID )
            {
                this->unselectCurrentAccount_();
            }     
        }

		AccountInterfacePtr account = it_find->second;
		       
		m_currentAccountID = _accountID;

		account->apply();

        if( m_accountProvider != nullptr )
        {
		    m_accountProvider->onSelectAccount( _accountID );
        }

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool AccountManager::hasCurrentAccount() const
	{
		return m_currentAccountID.empty() == false;
	}
	//////////////////////////////////////////////////////////////////////////
	const WString & AccountManager::getCurrentAccountID() const
	{
		return m_currentAccountID;
	}
	//////////////////////////////////////////////////////////////////////////
	AccountInterfacePtr AccountManager::getAccount( const WString& _accountID )
	{
		TMapAccounts::iterator it_found = m_accounts.find( _accountID );
		
		if( it_found == m_accounts.end() )
		{
			LOGGER_ERROR(m_serviceProvider)("AccountManager::getAccount account with ID '%ls' not found"
				, _accountID.c_str() 
				);

			return nullptr;
		}

		AccountInterfacePtr account = it_found->second;

		return account;
	}
	//////////////////////////////////////////////////////////////////////////
	void AccountManager::visitAccounts( AccountVisitorInterface * _visitor ) const
	{
		for( TMapAccounts::const_iterator
			it = m_accounts.begin(),
			it_end = m_accounts.end();
		it != it_end;
		++it )
		{
			const AccountInterfacePtr & account = it->second;

			const WString & accountID = account->getID();

			if( accountID == m_globalAccountID )
			{
				continue;
			}

			_visitor->onAccount( account );
		}		
	}
	//////////////////////////////////////////////////////////////////////////
	void AccountManager::setDefaultAccount( const WString & _accountID )
	{
		m_defaultAccountID = _accountID;
	}
	//////////////////////////////////////////////////////////////////////////
	const WString & AccountManager::getDefaultAccountID() const
	{
		return m_defaultAccountID;
	}
	//////////////////////////////////////////////////////////////////////////
	bool AccountManager::isCurrentDefaultAccount() const
	{
		if( m_defaultAccountID.empty() == true )
		{
			return false;
		}

		if( m_currentAccountID.empty() == true )
		{
			return false;
		}

		return m_defaultAccountID == m_currentAccountID;
	}
	//////////////////////////////////////////////////////////////////////////
	bool AccountManager::hasDefaultAccount() const
	{
		if( m_defaultAccountID.empty() == true )
		{
			return false;
		}

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	void AccountManager::setGlobalAccount( const WString & _accountID )
	{
		m_globalAccountID = _accountID;
	}
	//////////////////////////////////////////////////////////////////////////
	const WString & AccountManager::getGlobalAccountID() const
	{
		return m_globalAccountID;
	}
	//////////////////////////////////////////////////////////////////////////
	bool AccountManager::hasGlobalAccount() const
	{
		if( m_globalAccountID.empty() == true )
		{
			return false;
		}

		return true;		
	}
	//////////////////////////////////////////////////////////////////////////
	bool AccountManager::selectDefaultAccount()
	{
		if( m_defaultAccountID.empty() == true )
		{
			return false;
		}

		if( this->selectAccount( m_defaultAccountID ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("AccountManager::selectDefaultAccount invalid select account %ls"
                , m_defaultAccountID.c_str()
                );

            return false;
        }
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool AccountManager::loadAccount_( const AccountInterfacePtr & _account )
	{
		const WString & accountID = _account->getID();

		if( m_accountProvider != nullptr )
        {
            m_currentAccountID = accountID;
            m_accountProvider->onCreateAccount( accountID, m_globalAccountID == accountID );
			m_currentAccountID.clear();
        }

        if( _account->load() == false )
        {
            LOGGER_ERROR(m_serviceProvider)("AccountManager::loadAccount_ invalid load account %ls"
                , accountID.c_str()
                );

            return false;
        }       
        
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool AccountManager::loadAccounts()
	{        
		bool noLoadAccount = HAS_OPTION( m_serviceProvider, "noaccounts" );

		if( noLoadAccount == true )
		{
			return true;
		}

		FilePath accountsPath = CONFIG_VALUE( m_serviceProvider, "Game", "AccountsPath", STRINGIZE_FILEPATH_LOCAL( m_serviceProvider, "accounts.ini" ) );

		bool accountsExist = FILE_SERVICE(m_serviceProvider)
            ->existFile( CONST_STRING(m_serviceProvider, user), accountsPath, nullptr );

		if( accountsExist == false )
		{
			LOGGER_WARNING(m_serviceProvider)( "AccountManager::loadAccounts not exist accounts '%s'"
				, accountsPath.c_str()
				);

			return true;
		}
        	
		IniUtil::IniStore ini;
		if( IniUtil::loadIni( ini, CONST_STRING( m_serviceProvider, user ), accountsPath, m_serviceProvider ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("AccountManager::loadAccounts parsing accounts failed '%s'"
				, accountsPath.c_str()
				);

			return false;
		}

		//unsigned int playerCount;

		//config.getSettingUInt( L"SETTINGS", L"AccountCount", playerCount );
        if( IniUtil::getIniValue( ini, "SETTINGS", "AccountEnumerator", m_playerEnumerator, m_serviceProvider ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("AccountManager::loadAccounts get AccountEnumerator failed '%s'"
                , accountsPath.c_str()
                );

            return false;
        }

		if( IniUtil::getIniValue( ini, "SETTINGS", "GlobalAccountID", m_globalAccountID, m_serviceProvider ) == false )
		{
			LOGGER_INFO( m_serviceProvider )("AccountManager::loadAccounts get GlobalAccountID failed '%s'"
				, accountsPath.c_str()
				);
		}

		if( IniUtil::getIniValue( ini, "SETTINGS", "DefaultAccountID", m_defaultAccountID, m_serviceProvider ) == false )
        {
            LOGGER_INFO(m_serviceProvider)( "AccountManager::loadAccounts get DefaultAccountID failed '%s'"
                , accountsPath.c_str()
                );
        }

		WString selectAccountID;
		if( IniUtil::getIniValue( ini, "SETTINGS", "SelectAccountID", selectAccountID, m_serviceProvider ) == false )
        {
            LOGGER_INFO(m_serviceProvider)( "AccountManager::loadAccounts get SelectAccountID failed '%s'"
                , accountsPath.c_str()
                );
        }   
        
        TVectorWString values;
		if( IniUtil::getIniValue( ini, "ACCOUNTS", "Account", values, m_serviceProvider ) == false )
        {
            LOGGER_INFO(m_serviceProvider)( "AccountManager::loadAccounts get ACCOUNTS failed '%s'"
                , accountsPath.c_str()
                );
        }  

        AccountInterfacePtr validAccount = nullptr;

		for( TVectorWString::const_iterator
			it = values.begin(), 
			it_end = values.end();
		it != it_end;
		++it )
		{
			const WString & accountID = *it;

			AccountInterfacePtr account = this->newAccount_( accountID );

			if( account == nullptr )
			{
				LOGGER_ERROR( m_serviceProvider )("AccountManager::loadAccountsInfo invalid create account %ls"
					, accountID.c_str()
					);

				return false;
			}

			m_accounts.insert( std::make_pair( accountID, account ) );

            if( this->loadAccount_( account ) == false )
            {
                LOGGER_ERROR(m_serviceProvider)("AccountManager::loadAccountsInfo invalid load account '%ls'"
                    , accountID.c_str()
                    );

                continue;
            }

            validAccount = account;
		}

		if( selectAccountID.empty() == false )
		{
            LOGGER_INFO(m_serviceProvider)( "AccountManager::loadAccounts select account '%ls'"
                , selectAccountID.c_str()
                );

			if( this->selectAccount( selectAccountID ) == false )
            {
                LOGGER_ERROR(m_serviceProvider)("AccountManager::loadAccounts invalid set select account '%ls'"
                    , selectAccountID.c_str()
                    );

                return false;
            }
		}
        else if( m_defaultAccountID.empty() == false )
        {
            LOGGER_INFO(m_serviceProvider)( "AccountManager::loadAccounts set default account '%ls'"
                , m_defaultAccountID.c_str()
                );

            if( this->selectAccount( m_defaultAccountID ) == false )
            {
                LOGGER_ERROR(m_serviceProvider)("AccountManager::loadAccounts invalid set default account '%ls'"
                    , m_defaultAccountID.c_str()
                    );

                return false;
            }
        }
        else if( validAccount != nullptr )
        {
            const WString & accountID = validAccount->getID();

            LOGGER_WARNING(m_serviceProvider)( "AccountManager::loadAccounts set valid account '%ls'"
                , accountID.c_str()
                );

            if( this->selectAccount( accountID ) == false )
            {
                LOGGER_ERROR(m_serviceProvider)("AccountManager::loadAccounts invalid set valid account '%ls'"
                    , accountID.c_str()
                    );

                return false;
            }
        }
        else
        {
            LOGGER_INFO(m_serviceProvider)("AccountManager::loadAccounts invalid set any accounts"
                );
        }
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool AccountManager::saveAccounts()
	{
		FilePath accountsPath = CONFIG_VALUE( m_serviceProvider, "Game", "AccountsPath", STRINGIZE_FILEPATH_LOCAL( m_serviceProvider, "accounts.ini" ) );

		OutputStreamInterfacePtr file = FILE_SERVICE(m_serviceProvider)
            ->openOutputFile( CONST_STRING(m_serviceProvider, user), accountsPath );

		if( file == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("AccountManager::saveAccountsInfo can't open file for writing. Accounts '%s' settings not saved"
				, accountsPath.c_str()
				);

			return false;
		}

        IniUtil::writeIniSection( m_serviceProvider, file, "[SETTINGS]" );

		if( m_globalAccountID.empty() == false )
		{
			IniUtil::writeIniSetting( m_serviceProvider, file, "GlobalAccountID", m_globalAccountID );
		}

		if( m_defaultAccountID.empty() == false )
		{
            IniUtil::writeIniSetting( m_serviceProvider, file, "DefaultAccountID", m_defaultAccountID );
		}

		if( m_currentAccountID.empty() == false )
		{
            IniUtil::writeIniSetting( m_serviceProvider, file, "SelectAccountID", m_currentAccountID );
		}

        WString AccountEnumerator;
        Utils::unsignedToWString( m_playerEnumerator, AccountEnumerator );

        IniUtil::writeIniSetting( m_serviceProvider, file, "AccountEnumerator", AccountEnumerator );

        IniUtil::writeIniSection( m_serviceProvider, file, "[ACCOUNTS]" );

		for( TMapAccounts::iterator 
			it = m_accounts.begin(), 
			it_end = m_accounts.end();
		it != it_end;
		++it )
		{
			const WString & accountID = it->first;

            IniUtil::writeIniSetting( m_serviceProvider, file, "Account", accountID );
		}

        for( TMapAccounts::iterator 
            it = m_accounts.begin(), 
            it_end = m_accounts.end();
        it != it_end;
        ++it )
        {
			const AccountInterfacePtr & account = it->second;

            if( account->save() == false )
            {
                LOGGER_ERROR(m_serviceProvider)("AccountManager::finalize invalid save account %ls:%s"
                    , account->getID().c_str()
                    , account->getFolder().c_str()
                    );

                return false;
            }
        }

        return true;
	}
}