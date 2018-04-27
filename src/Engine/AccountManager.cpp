#include "AccountManager.h"
#include "Account.h"

#include "Interface/ApplicationInterface.h"
#include "Interface/OptionsInterface.h"
#include "Interface/UnicodeInterface.h"
#include "Interface/FileSystemInterface.h"
#include "Interface/StringizeInterface.h"
#include "Interface/ConfigInterface.h"

#include "Factory/FactoryPool.h"

#include "Logger/Logger.h"

#include "Core/IniUtil.h"
#include "Core/String.h"
#include "Core/UID.h"

#include "Config/Typedef.h"
#include "Config/Stringstream.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( AccountService, Mengine::AccountManager );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
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
		LOGGER_INFO("Initializing Account manager..."
			);
		
        m_factoryAccounts = new FactoryPool<Account, 8>();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AccountManager::_finalize()
    {
        LOGGER_WARNING("AccountManager::finalize save accounts"
            );

        ConstString lastAccount = m_currentAccountID;
        this->unselectCurrentAccount_();

		m_currentAccountID = lastAccount;

        this->saveAccounts();

		m_currentAccountID.clear();
        m_accounts.clear();

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryAccounts );
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

		Stringstream streamAccountID;
		streamAccountID << "Player_" << new_playerID;

		ConstString accountID = Helper::stringizeString( streamAccountID.str() );
        
		AccountInterfacePtr account = this->createAccount_( accountID );

		String uid = Helper::makeUID( 20 );

		account->setUID( uid );

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

		Stringstream streamAccountID;
		streamAccountID << "Global_" << new_playerID;

		ConstString accountID = Helper::stringizeString( streamAccountID.str() );

		AccountInterfacePtr account = this->createGlobalAccount_( accountID );

		if( account == nullptr )
		{
			return nullptr;
		}

		return account;
	}
	//////////////////////////////////////////////////////////////////////////
	AccountInterfacePtr AccountManager::createAccount_( const ConstString& _accountID )
	{
		TMapAccounts::iterator it_find = m_accounts.find( _accountID );

		if( it_find != m_accounts.end() )
		{
			LOGGER_ERROR("AccountManager::createAccount_: Account with ID '%s' already exist. Account not created"
				, _accountID.c_str() 
				);

			return nullptr;
		}

		this->unselectCurrentAccount_();
				
		AccountInterfacePtr newAccount = this->newAccount_( _accountID );

        if( newAccount == nullptr )
        {
            LOGGER_ERROR("AccountManager::createAccount_: Account with ID '%s' invalid create. Account not created"
                , _accountID.c_str() 
                );

            return nullptr;
        }

		String uid = Helper::makeUID( 20 );

		newAccount->setUID( uid );
        
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
	AccountInterfacePtr AccountManager::createGlobalAccount_( const ConstString& _accountID )
	{
		TMapAccounts::iterator it_find = m_accounts.find( _accountID );

		if( it_find != m_accounts.end() )
		{
			LOGGER_ERROR("AccountManager::createGlobalAccount_: Account with ID '%s' already exist. Account not created"
				, _accountID.c_str()
				);

			return nullptr;
		}
				
		AccountInterfacePtr newAccount = this->newAccount_( _accountID );

		if( newAccount == nullptr )
		{
			LOGGER_ERROR("AccountManager::createGlobalAccount_: Account with ID '%s' invalid create. Account not created"
				, _accountID.c_str()
				);

			return nullptr;
		}

		String uid = Helper::makeUID( 20 );

		newAccount->setUID( uid );

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

        ConstString currentAccount = m_currentAccountID;
        
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
    AccountInterfacePtr AccountManager::newAccount_( const ConstString& _accountID )
    {
        FileGroupInterfacePtr fileGroup = FILE_SERVICE()
            ->getFileGroup( STRINGIZE_STRING_LOCAL( "user" ) );

        PathString accountString;
        accountString.append( _accountID );
        accountString.append( '/' );

        FilePath accountPath = Helper::stringizeFilePath( accountString );

        if( fileGroup->createDirectory( accountPath ) == false )
        {
            LOGGER_ERROR("AccountManager::createAccount_: Account '%s' failed create directory"
                , accountPath.c_str()
                );

            return nullptr;
        }        

        AccountPtr newAccount = m_factoryAccounts->createObject();

        uint32_t projectVersion = APPLICATION_SERVICE()
			->getProjectVersion();

		if( newAccount->initialize( _accountID, accountPath, projectVersion ) == false )
		{
			return nullptr;				 
		}

        return newAccount;
    }
	//////////////////////////////////////////////////////////////////////////
	void AccountManager::deleteAccount( const ConstString& _accountID )
	{
		TMapAccounts::iterator it_find = m_accounts.find( _accountID );

		if( it_find == m_accounts.end() )
		{
			LOGGER_ERROR("AccountManager::deleteAccount Can't delete account '%s'. There is no account with such ID"
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
	bool AccountManager::selectAccount( const ConstString& _accountID )
	{
		TMapAccounts::iterator it_find = m_accounts.find( _accountID );

		if( it_find == m_accounts.end() )
		{
			LOGGER_ERROR("AccountManager::selectAccount Can't select account '%s'. There is no account with such ID"
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
	const ConstString & AccountManager::getCurrentAccountID() const
	{
		return m_currentAccountID;
	}
	//////////////////////////////////////////////////////////////////////////
	AccountInterfacePtr AccountManager::getAccount( const ConstString& _accountID )
	{
		TMapAccounts::iterator it_found = m_accounts.find( _accountID );
		
		if( it_found == m_accounts.end() )
		{
			LOGGER_ERROR("AccountManager::getAccount account with ID '%s' not found"
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

			const ConstString & accountID = account->getID();

			if( accountID == m_globalAccountID )
			{
				continue;
			}

			_visitor->onAccount( account );
		}		
	}
	//////////////////////////////////////////////////////////////////////////
	void AccountManager::setDefaultAccount( const ConstString & _accountID )
	{
		m_defaultAccountID = _accountID;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & AccountManager::getDefaultAccountID() const
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
	void AccountManager::setGlobalAccount( const ConstString & _accountID )
	{
		m_globalAccountID = _accountID;
	}
	//////////////////////////////////////////////////////////////////////////
	const ConstString & AccountManager::getGlobalAccountID() const
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
            LOGGER_ERROR("AccountManager::selectDefaultAccount invalid select account %s"
                , m_defaultAccountID.c_str()
                );

            return false;
        }
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool AccountManager::loadAccount_( const AccountInterfacePtr & _account )
	{
		const ConstString & accountID = _account->getID();

		if( m_accountProvider != nullptr )
        {
            m_currentAccountID = accountID;
            m_accountProvider->onCreateAccount( accountID, m_globalAccountID == accountID );
			m_currentAccountID.clear();
        }

        if( _account->load() == false )
        {
            LOGGER_ERROR("AccountManager::loadAccount_ invalid load account %s"
                , accountID.c_str()
                );

            return false;
        }       
        
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool AccountManager::loadAccounts()
	{        
		bool noLoadAccount = HAS_OPTION( "noaccounts" );

		if( noLoadAccount == true )
		{
			return true;
		}

		FilePath accountsPath = CONFIG_VALUE( "Game", "AccountsPath", STRINGIZE_FILEPATH_LOCAL( "accounts.ini" ) );

		bool accountsExist = FILE_SERVICE()
            ->existFile( STRINGIZE_STRING_LOCAL( "user" ), accountsPath, nullptr );

		if( accountsExist == false )
		{
			LOGGER_WARNING( "AccountManager::loadAccounts not exist accounts '%s'"
				, accountsPath.c_str()
				);

			return true;
		}
        	
		IniUtil::IniStore ini;
        if( IniUtil::loadIni( ini, STRINGIZE_STRING_LOCAL( "user" ), accountsPath ) == false )
		{
			LOGGER_ERROR("AccountManager::loadAccounts parsing accounts failed '%s'"
				, accountsPath.c_str()
				);

			return false;
		}

		//unsigned int playerCount;

		//config.getSettingUInt( L"SETTINGS", L"AccountCount", playerCount );
        if( IniUtil::getIniValue( ini, "SETTINGS", "AccountEnumerator", m_playerEnumerator ) == false )
        {
            LOGGER_ERROR("AccountManager::loadAccounts get AccountEnumerator failed '%s'"
                , accountsPath.c_str()
                );

            return false;
        }

		if( IniUtil::getIniValue( ini, "SETTINGS", "GlobalAccountID", m_globalAccountID ) == false )
		{
			LOGGER_INFO("AccountManager::loadAccounts get GlobalAccountID failed '%s'"
				, accountsPath.c_str()
				);
		}

		if( IniUtil::getIniValue( ini, "SETTINGS", "DefaultAccountID", m_defaultAccountID ) == false )
        {
            LOGGER_INFO( "AccountManager::loadAccounts get DefaultAccountID failed '%s'"
                , accountsPath.c_str()
                );
        }

		ConstString selectAccountID;
		if( IniUtil::getIniValue( ini, "SETTINGS", "SelectAccountID", selectAccountID ) == false )
        {
            LOGGER_INFO( "AccountManager::loadAccounts get SelectAccountID failed '%s'"
                , accountsPath.c_str()
                );
        }   
        
        TVectorConstString values;
		if( IniUtil::getIniValue( ini, "ACCOUNTS", "Account", values ) == false )
        {
            LOGGER_INFO( "AccountManager::loadAccounts get ACCOUNTS failed '%s'"
                , accountsPath.c_str()
                );
        }  

        AccountInterfacePtr validAccount = nullptr;

        for( const ConstString & accountID : values )
		{
			AccountInterfacePtr account = this->newAccount_( accountID );

			if( account == nullptr )
			{
				LOGGER_ERROR("AccountManager::loadAccountsInfo invalid create account %s"
					, accountID.c_str()
					);

				return false;
			}

			m_accounts.insert( std::make_pair( accountID, account ) );

            if( this->loadAccount_( account ) == false )
            {
                LOGGER_ERROR("AccountManager::loadAccountsInfo invalid load account '%s'"
                    , accountID.c_str()
                    );

                continue;
            }

            validAccount = account;
		}

		if( selectAccountID.empty() == false )
		{
            LOGGER_INFO( "AccountManager::loadAccounts select account '%s'"
                , selectAccountID.c_str()
                );

			if( this->selectAccount( selectAccountID ) == false )
            {
                LOGGER_ERROR("AccountManager::loadAccounts invalid set select account '%s'"
                    , selectAccountID.c_str()
                    );

                return false;
            }
		}
        else if( m_defaultAccountID.empty() == false )
        {
            LOGGER_INFO( "AccountManager::loadAccounts set default account '%s'"
                , m_defaultAccountID.c_str()
                );

            if( this->selectAccount( m_defaultAccountID ) == false )
            {
                LOGGER_ERROR("AccountManager::loadAccounts invalid set default account '%s'"
                    , m_defaultAccountID.c_str()
                    );

                return false;
            }
        }
        else if( validAccount != nullptr )
        {
            const ConstString & accountID = validAccount->getID();

            LOGGER_WARNING( "AccountManager::loadAccounts set valid account '%s'"
                , accountID.c_str()
                );

            if( this->selectAccount( accountID ) == false )
            {
                LOGGER_ERROR("AccountManager::loadAccounts invalid set valid account '%s'"
                    , accountID.c_str()
                    );

                return false;
            }
        }
        else
        {
            LOGGER_INFO("AccountManager::loadAccounts invalid set any accounts"
                );
        }
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool AccountManager::saveAccounts()
	{
		FilePath accountsPath = CONFIG_VALUE( "Game", "AccountsPath", STRINGIZE_FILEPATH_LOCAL( "accounts.ini" ) );

		OutputStreamInterfacePtr file = FILE_SERVICE()
            ->openOutputFile( STRINGIZE_STRING_LOCAL( "user" ), accountsPath );

		if( file == nullptr )
		{
			LOGGER_ERROR("AccountManager::saveAccountsInfo can't open file for writing. Accounts '%s' settings not saved"
				, accountsPath.c_str()
				);

			return false;
		}

        IniUtil::writeIniSection( file, "[SETTINGS]" );

		if( m_globalAccountID.empty() == false )
		{
			IniUtil::writeIniSetting( file, "GlobalAccountID", m_globalAccountID );
		}

		if( m_defaultAccountID.empty() == false )
		{
            IniUtil::writeIniSetting( file, "DefaultAccountID", m_defaultAccountID );
		}

		if( m_currentAccountID.empty() == false )
		{
            IniUtil::writeIniSetting( file, "SelectAccountID", m_currentAccountID );
		}

        WString AccountEnumerator;
        Helper::unsignedToWString( m_playerEnumerator, AccountEnumerator );

        IniUtil::writeIniSetting( file, "AccountEnumerator", AccountEnumerator );

        IniUtil::writeIniSection( file, "[ACCOUNTS]" );

		for( TMapAccounts::iterator 
			it = m_accounts.begin(), 
			it_end = m_accounts.end();
		it != it_end;
		++it )
		{
			const ConstString & accountID = it->first;

            IniUtil::writeIniSetting( file, "Account", accountID );
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
                LOGGER_ERROR("AccountManager::finalize invalid save account %s:%s"
                    , account->getID().c_str()
                    , account->getFolder().c_str()
                    );

                return false;
            }
        }

        return true;
	}
}