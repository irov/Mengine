#	include "AccountManager.h"
#	include "Account.h"

#   include "Interface/ApplicationInterface.h"
#   include "Interface/OptionsInterface.h"
#   include "Interface/UnicodeInterface.h"
#   include "Interface/FileSystemInterface.h"
#   include "Interface/StringizeInterface.h"
#   include "Interface/ConfigInterface.h"

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
		, m_currentAccount(nullptr)
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
		
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AccountManager::_finalize()
    {
        LOGGER_WARNING(m_serviceProvider)("AccountManager::finalize save accounts"
            );

        AccountInterfacePtr lastAccount = m_currentAccount;
        this->unselectCurrentAccount_();

        m_currentAccount = lastAccount;

        this->saveAccounts();

        m_currentAccount = nullptr;
        m_accounts.clear();
    }
	//////////////////////////////////////////////////////////////////////////
	void AccountManager::setAccountProviderInterface( AccountProviderInterface * _accountProvider )
	{
		m_accountProvider = _accountProvider;
	}
	//////////////////////////////////////////////////////////////////////////
	AccountInterfacePtr AccountManager::createAccount()
	{
        uint32_t new_playerID = ++m_playerEnumerator;

		//wchar_t bufferAccountID[128];
		//swprintf( bufferAccountID, 128, L"Player_%d", new_playerID );
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
        
		m_currentAccount = newAccount;

        if( m_accountProvider != nullptr )
        {
		    m_accountProvider->onCreateAccount( _accountID );
        }
        
		m_accounts.insert( std::make_pair( _accountID, newAccount ) );

		m_currentAccount->apply();

		if( m_accountProvider != nullptr )
		{
			m_accountProvider->onSelectAccount( _accountID );
		}

        return newAccount;
	}
    //////////////////////////////////////////////////////////////////////////
    void AccountManager::unselectCurrentAccount_()
    {
        if( m_currentAccount == nullptr )
        {
            return;
        }

		AccountInterfacePtr currentAccount = m_currentAccount;
        
        if( m_accountProvider != nullptr )
        {
            const WString & name = m_currentAccount->getName();

            m_accountProvider->onUnselectAccount( name );
        }

		if( m_currentAccount == currentAccount )
		{
			m_currentAccount = nullptr;
		}
    }
    //////////////////////////////////////////////////////////////////////////
    AccountInterfacePtr AccountManager::newAccount_( const WString& _accountID )
    {
        String utf8_path;
        if( Helper::unicodeToUtf8( m_serviceProvider, _accountID, utf8_path ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("AccountManager::newAccount_ can't convert accountID '%ls' to utf8"
                , _accountID.c_str() 
                );

            return nullptr;
        }

        ConstString folder = Helper::stringizeString( m_serviceProvider, utf8_path );

        if( FILE_SERVICE(m_serviceProvider)
            ->existDirectory( CONST_STRING(m_serviceProvider, user), folder ) == false )
        {
            if( FILE_SERVICE(m_serviceProvider)
                ->createDirectory( CONST_STRING(m_serviceProvider, user), folder ) == false )
            {
                LOGGER_ERROR(m_serviceProvider)("AccountManager::createAccount_: Account '%ls' failed create directory '%s'"
                    , _accountID.c_str() 
                    , folder.c_str()
                    );

                return nullptr;
            }
        }

        AccountPtr newAccount = m_factoryAccounts.createObject();

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

		if( m_currentAccount != nullptr )
		{
			const WString & name = m_currentAccount->getName();

            if( name == _accountID )
			{
                this->unselectCurrentAccount_();
			}
		}

		const FilePath & folder = account->getFolder();

        if( FILE_SERVICE(m_serviceProvider)
            ->existDirectory( CONST_STRING(m_serviceProvider, user), folder ) == true )
        {
		    FILE_SERVICE(m_serviceProvider)
                ->removeDirectory( CONST_STRING(m_serviceProvider, user), folder );
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

        if( m_currentAccount != nullptr )
        {
            const WString & currAccountId = m_currentAccount->getName();

            if( currAccountId != _accountID )
            {
                this->unselectCurrentAccount_();
            }     
        }

		AccountInterfacePtr account = it_find->second;
		       
        m_currentAccount = account;

		m_currentAccount->apply();

        if( m_accountProvider != nullptr )
        {
		    m_accountProvider->onSelectAccount( _accountID );
        }

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool AccountManager::hasCurrentAccount() const
	{
		return m_currentAccount != nullptr;
	}
	//////////////////////////////////////////////////////////////////////////
	const AccountInterfacePtr & AccountManager::getCurrentAccount()
	{
		return m_currentAccount;
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

			_visitor->onAccount( account );
		}		
	}
	//////////////////////////////////////////////////////////////////////////
	void AccountManager::setDefaultAccount( const WString & _accountID )
	{
		m_defaultAccountID = _accountID;
	}
	//////////////////////////////////////////////////////////////////////////
	const WString & AccountManager::getDefaultAccount() const
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

		if( m_currentAccount == nullptr )
		{
			return false;
		}

		const WString & accountID = m_currentAccount->getName();

		return m_defaultAccountID == accountID;
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
	AccountInterfacePtr AccountManager::loadAccount_( const WString& _accountID )
	{
		AccountInterfacePtr account = this->newAccount_( _accountID );

        if( account == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("AccountManager::loadAccount_ invalid create account %ls"
                , _accountID.c_str()
                );

            return nullptr;
        }
        
        if( m_accountProvider != nullptr )
        {
            m_currentAccount = account;
            m_accountProvider->onCreateAccount( _accountID );
            m_currentAccount = nullptr;
        }

        if( account->load() == false )
        {
            LOGGER_ERROR(m_serviceProvider)("AccountManager::loadAccount_ invalid load account %ls"
                , _accountID.c_str()
                );

            return nullptr;
        }       
        
		return account;
	}
	//////////////////////////////////////////////////////////////////////////
	bool AccountManager::loadAccounts()
	{        
		bool noLoadAccount = HAS_OPTION( m_serviceProvider, "noaccounts" );

		if( noLoadAccount == true )
		{
			return true;
		}

		FilePath accountsPath = CONFIG_VALUE( m_serviceProvider, "Game", "AccountsPath", STRINGIZE_STRING_LOCAL( m_serviceProvider, "accounts.ini" ) );

		bool accountsExist = FILE_SERVICE(m_serviceProvider)
            ->existFile( CONST_STRING(m_serviceProvider, user), accountsPath, nullptr );

		if( accountsExist == false )
		{
			LOGGER_WARNING(m_serviceProvider)( "AccountManager::loadAccounts not exist accounts '%s'"
				, accountsPath.c_str()
				);

			return true;
		}
        
		InputStreamInterfacePtr file = FILE_SERVICE(m_serviceProvider)
            ->openInputFile( CONST_STRING(m_serviceProvider, user), accountsPath, false );

        if( file == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("AccountManager::loadAccounts open accounts failed '%s'"
                , accountsPath.c_str()
                );

            return false;
        }
		
		IniUtil::IniStore ini;
		if( IniUtil::loadIni( ini, file, m_serviceProvider ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("AccountManager::loadAccounts parsing accounts failed '%s'"
				, accountsPath.c_str()
				);

			return false;
		}

		file = nullptr;

		//unsigned int playerCount;

		//config.getSettingUInt( L"SETTINGS", L"AccountCount", playerCount );
        if( IniUtil::getIniValue( ini, "SETTINGS", "AccountEnumerator", m_playerEnumerator, m_serviceProvider ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("AccountManager::loadAccounts get AccountEnumerator failed '%s'"
                , accountsPath.c_str()
                );

            return false;
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
			const WString & name = *it;

			AccountInterfacePtr account = this->loadAccount_( name );

            if( account == nullptr )
            {
                LOGGER_ERROR(m_serviceProvider)("AccountManager::loadAccountsInfo invalid load account '%ls'"
                    , name.c_str()
                    );

                continue;
            }

            validAccount = account;

			m_accounts.insert( std::make_pair( name, account ) );
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
            const WString & accountID = validAccount->getName();

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
		FilePath m_accountsPath = CONFIG_VALUE( m_serviceProvider, "Game", "AccountsPath", STRINGIZE_STRING_LOCAL( m_serviceProvider, "accounts.ini" ) );

		OutputStreamInterfacePtr file = FILE_SERVICE(m_serviceProvider)
            ->openOutputFile( CONST_STRING(m_serviceProvider, user), m_accountsPath );

		if( file == nullptr )
		{
			LOGGER_ERROR(m_serviceProvider)("AccountManager::saveAccountsInfo can't open file for writing. Accounts '%s' settings not saved"
				, m_accountsPath.c_str()
				);

			return false;
		}

        IniUtil::writeIniSection( m_serviceProvider, file, "[SETTINGS]" );

		if( m_defaultAccountID.empty() == false )
		{
            IniUtil::writeIniSetting( m_serviceProvider, file, "DefaultAccountID", m_defaultAccountID );
		}

		if( m_currentAccount != nullptr )
		{
			const WString & name = m_currentAccount->getName();

            IniUtil::writeIniSetting( m_serviceProvider, file, "SelectAccountID", name );
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
                    , account->getName().c_str()
                    , account->getFolder().c_str()
                    );

                return false;
            }
        }

        return true;
	}
}