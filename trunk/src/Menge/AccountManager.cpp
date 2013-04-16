#	include "AccountManager.h"
#	include "Account.h"

#   include "Interface/UnicodeInterface.h"
#   include "Interface/FileSystemInterface.h"
#   include "Interface/StringizeInterface.h"

#	include "Logger/Logger.h"

#	include "Consts.h"

#	include "Core/String.h"
#	include "Core/File.h"
#	include "Core/UnicodeFormat.h"

#	include "Config/Typedef.h"

#	include "ConfigFile/ConfigFile.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( AccountService, Menge::AccountServiceInterface, Menge::AccountManager );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	AccountManager::AccountManager()
		: m_accountListener(NULL)
        , m_serviceProvider(NULL)
		, m_currentAccount(NULL)
		, m_playerEnumerator(0)
	{
	}
	//////////////////////////////////////////////////////////////////////////
	AccountManager::~AccountManager()
	{
	}
    //////////////////////////////////////////////////////////////////////////
    void AccountManager::setServiceProvider( ServiceProviderInterface * _serviceProvider )
    {
        m_serviceProvider = _serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    ServiceProviderInterface * AccountManager::getServiceProvider() const
    {
        return m_serviceProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AccountManager::initialize( const FilePath & _accountsPath, AccountServiceListener * _listener )
    {
        m_accountsPath = _accountsPath;
        m_accountListener = _listener;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AccountManager::finalize()
    {
        AccountInterface * lastAccount = m_currentAccount;
        this->unselectCurrentAccount_();

        m_currentAccount = lastAccount;
        this->saveAccountsInfo();

        m_currentAccount = NULL;
               

        for( TMapAccounts::iterator 
            it = m_accounts.begin(), 
            it_end = m_accounts.end();
        it != it_end;
        ++it )
        {
            AccountInterface * account = it->second;
            delete static_cast<Account *>(account);
        }

        m_accounts.clear();
    }
	//////////////////////////////////////////////////////////////////////////
	AccountInterface * AccountManager::createAccount()
	{
		WString accountID = (UnicodeFormat(L"Player_%d") % m_playerEnumerator).str();

		Account * account = this->createAccount_( accountID );

        if( account == NULL )
        {
            return NULL;
        }
		
		++m_playerEnumerator;

        if( this->saveAccountsInfo() == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "AccountManager::createAccount: Account '%ls' failed to saveAccountsInfo"
                , accountID.c_str()
                );

            return NULL;
        }
		
		return account;
	}
	//////////////////////////////////////////////////////////////////////////
	void AccountManager::addSetting( const WString& _setting, const WString& _defaultValue, PyObject* _applyFunc )
	{
		if( m_currentAccount == 0 )
		{
            LOGGER_ERROR(m_serviceProvider)( "AccountManager::addSetting: not setup currentAccount '%ls'"
                , _setting.c_str()
                );

			return;
		}

		m_currentAccount->addSetting( _setting, _defaultValue, _applyFunc );		
	}
	//////////////////////////////////////////////////////////////////////////
	void AccountManager::changeSetting( const WString & _setting, const WString & _value )
	{
		if( m_currentAccount == 0 )
		{
			LOGGER_ERROR(m_serviceProvider)("AccountManager::changeSetting not setup currentAccount [%ls, %ls]"
				, _setting.c_str()
				, _value.c_str()
				);

			return;
		}
        
		m_currentAccount->changeSetting( _setting, _value );
	}
	//////////////////////////////////////////////////////////////////////////
	Account * AccountManager::createAccount_( const WString& _accountID )
	{
		TMapAccounts::iterator it_find = m_accounts.find( _accountID );
		if( it_find != m_accounts.end() )
		{
			LOGGER_ERROR(m_serviceProvider)( "AccountManager::createAccount_: Account with ID '%ls' already exist. Account not created"
				, _accountID.c_str() 
				);

			return NULL;
		}

        this->unselectCurrentAccount_();
				
		Account * newAccount = this->newAccount_( _accountID );

        if( newAccount == NULL )
        {
            LOGGER_ERROR(m_serviceProvider)( "AccountManager::createAccount_: Account with ID '%ls' invalid create. Account not created"
                , _accountID.c_str() 
                );

            return NULL;
        }

		m_accounts.insert( std::make_pair( _accountID, newAccount ) );

		m_currentAccount = newAccount;

        if( m_accountListener )
        {
		    m_accountListener->onCreateAccount( _accountID );
        }

		const FilePath & folder = newAccount->getFolder();

		if( FILE_SERVICE(m_serviceProvider)->createDirectory( CONST_STRING(m_serviceProvider, user), folder ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "AccountManager::createAccount_: Account '%ls' failed create directory '%s'"
                , _accountID.c_str() 
                , folder.c_str()
                );

            return NULL;
        }

		if( newAccount->save() == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "AccountManager::createAccount_: Account '%ls' failed to save"
                , _accountID.c_str()
                );

            return NULL;
        }

        return newAccount;
	}
    //////////////////////////////////////////////////////////////////////////
    void AccountManager::unselectCurrentAccount_()
    {
        if( m_currentAccount == NULL )
        {
            return;
        }
        
        if( m_accountListener != NULL )
        {
            const WString & name = m_currentAccount->getName();
            m_accountListener->onUnselectAccount( name );
        }        

        m_currentAccount = NULL;
    }
    //////////////////////////////////////////////////////////////////////////
    Account * AccountManager::newAccount_( const WString& _accountID )
    {
        String utf8_path;

        if( Helper::unicodeToUtf8( m_serviceProvider, _accountID, utf8_path ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "AccountManager::newAccount_ can't convert accountID '%ls' to utf8"
                , _accountID.c_str() 
                );

            return NULL;
        }

        Account* newAccount = new Account(m_serviceProvider, _accountID);

        FilePath folder = Helper::stringizeString( m_serviceProvider, utf8_path );

        newAccount->setFolder( folder );

        return newAccount;
    }
	//////////////////////////////////////////////////////////////////////////
	void AccountManager::deleteAccount( const WString& _accountID )
	{
		TMapAccounts::iterator it_find = m_accounts.find( _accountID );

		if( it_find == m_accounts.end() )
		{
			LOGGER_ERROR(m_serviceProvider)( "AccountManager::deleteAccount Can't delete account '%ls'. There is no account with such ID"
				, _accountID.c_str() 
				);

			return;
		}

		if( m_currentAccount )
		{
			const WString & name = m_currentAccount->getName();

            if( name == _accountID )
			{
                this->unselectCurrentAccount_();
			}
		}

		const FilePath & folder = it_find->second->getFolder();

		FILE_SERVICE(m_serviceProvider)
            ->removeDirectory( CONST_STRING(m_serviceProvider, user), folder );

		delete it_find->second;

		m_accounts.erase( it_find );

		this->saveAccountsInfo();

        if( m_accountListener != NULL )
        {
            m_accountListener->onDeleteAccount( _accountID );
        }  
	}
	//////////////////////////////////////////////////////////////////////////
	bool AccountManager::selectAccount( const WString& _accountID )
	{
		TMapAccounts::iterator it_find = m_accounts.find( _accountID );

		if( it_find == m_accounts.end() )
		{
			LOGGER_ERROR(m_serviceProvider)( "AccountManager::selectAccount Can't select account '%ls'. There is no account with such ID"
				, _accountID.c_str() 
				);
			
			return false;
		}

        if( m_currentAccount != NULL )
        {
            const WString & currAccountId = m_currentAccount->getName();

            if( currAccountId != _accountID )
            {
                this->unselectCurrentAccount_();
            }
        }

        AccountInterface * account = it_find->second;
		
        if( account->load() == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "AccountManager::selectAccount Can't select account '%ls'. account not load"
                , _accountID.c_str() 
                );

            return false;
        }
        
        m_currentAccount = account;

		m_currentAccount->apply();

		if( this->saveAccountsInfo() == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "AccountManager::selectAccount Can't select account '%ls'. invalid saveAccountsInfo"
                , _accountID.c_str() 
                );

            return false;
        }

        if( m_accountListener )
        {
		    m_accountListener->onSelectAccount( _accountID );
        }

		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool AccountManager::hasCurrentAccount() const
	{
		return m_currentAccount != NULL;
	}
	//////////////////////////////////////////////////////////////////////////
	AccountInterface * AccountManager::getCurrentAccount()
	{
		return m_currentAccount;
	}
	//////////////////////////////////////////////////////////////////////////
	AccountInterface * AccountManager::getAccount( const WString& _accountID )
	{
		TMapAccounts::iterator it_find = m_accounts.find( _accountID );
		
		if( it_find == m_accounts.end() )
		{
			LOGGER_ERROR(m_serviceProvider)( "Account with ID '%ls' not found"
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
	Account * AccountManager::loadAccount_( const WString& _accountID )
	{
		Account* account = this->newAccount_( _accountID );

        if( account == NULL )
        {
            LOGGER_ERROR(m_serviceProvider)("AccountManager::loadAccount_ invalid create account %ls"
                , _accountID.c_str()
                );

            return NULL;
        }
        
        if( m_accountListener )
        {
            m_currentAccount = account;
            m_accountListener->onCreateAccount( _accountID );
            m_currentAccount = NULL;
        }

        if( account->load() == false )
        {
		    delete account;

            LOGGER_ERROR(m_serviceProvider)("AccountManager::loadAccount_ invalid load account %ls"
                , _accountID.c_str()
                );

            return 0;
        }       
        
		return account;
	}
	//////////////////////////////////////////////////////////////////////////
	bool AccountManager::loadAccountsInfo()
	{
		bool accountsExist = FILE_SERVICE(m_serviceProvider)
            ->existFile( CONST_STRING(m_serviceProvider, user), m_accountsPath, NULL );

		if( accountsExist == false )
		{
			LOGGER_WARNING(m_serviceProvider)( "AccountManager::loadAccounts not exist accounts '%s'"
				, m_accountsPath.c_str()
				);

			return true;
		}
        
		InputStreamInterface* file = FILE_SERVICE(m_serviceProvider)
            ->openInputFile( CONST_STRING(m_serviceProvider, user), m_accountsPath );

        if( file == NULL )
        {
            LOGGER_ERROR(m_serviceProvider)( "AccountManager::loadAccounts open accounts failed '%s'"
                , m_accountsPath.c_str()
                );

            return false;
        }
		
        ConfigFile config;
		if( config.load( file ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)( "AccountManager::loadAccounts parsing accounts failed '%s'"
				, m_accountsPath.c_str()
				);

			return false;
		}

		file->destroy();

		//unsigned int playerCount;

		//config.getSettingUInt( L"SETTINGS", L"AccountCount", playerCount );
		if( config.getSettingUInt( L"SETTINGS", L"AccountEnumerator", m_playerEnumerator ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "AccountManager::loadAccounts get AccountEnumerator failed '%s'"
                , m_accountsPath.c_str()
                );

            return false;
        }

		if( config.getSetting( L"SETTINGS", L"DefaultAccountID", m_defaultAccountID ) == false )
        {
            LOGGER_INFO(m_serviceProvider)( "AccountManager::loadAccounts get DefaultAccountID failed '%s'"
                , m_accountsPath.c_str()
                );
        }           

		WString selectAccountID;
		if( config.getSetting( L"SETTINGS", L"SelectAccountID", selectAccountID ) == false )
        {
            LOGGER_INFO(m_serviceProvider)( "AccountManager::loadAccounts get SelectAccountID failed '%s'"
                , m_accountsPath.c_str()
                );
        }   
        
        TVectorWString values;
		if( config.getSettings( L"ACCOUNTS", L"Account", values ) == false )
        {
            LOGGER_INFO(m_serviceProvider)( "AccountManager::loadAccounts get ACCOUNTS failed '%s'"
                , m_accountsPath.c_str()
                );
        }  

        Account * validAccount = 0;

		for( TVectorWString::const_iterator
			it = values.begin(), 
			it_end = values.end();
		it != it_end;
		++it )
		{
			const WString & name = *it;
			//config.indexSetting( it, "ACCOUNTS", "Account", accountID );

			Account * account = this->loadAccount_( name );

            if( account == 0 )
            {
                LOGGER_ERROR(m_serviceProvider)( "AccountManager::loadAccountsInfo invalid load account '%ls'"
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
                LOGGER_ERROR(m_serviceProvider)( "AccountManager::loadAccounts invalid set select account '%ls'"
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
                LOGGER_ERROR(m_serviceProvider)( "AccountManager::loadAccounts invalid set default account '%ls'"
                    , m_defaultAccountID.c_str()
                    );

                return false;
            }
        }
        else if( validAccount != NULL )
        {
            const WString & accountID = validAccount->getName();

            LOGGER_WARNING(m_serviceProvider)( "AccountManager::loadAccounts set valid account '%ls'"
                , accountID.c_str()
                );

            if( this->selectAccount( accountID ) == false )
            {
                LOGGER_ERROR(m_serviceProvider)( "AccountManager::loadAccounts invalid set valid account '%ls'"
                    , accountID.c_str()
                    );

                return false;
            }
        }
        else
        {
            LOGGER_ERROR(m_serviceProvider)( "AccountManager::loadAccounts invalid set any accounts"
                );

            return false;
        }
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool AccountManager::saveAccountsInfo()
	{
		OutputStreamInterface* file = FILE_SERVICE(m_serviceProvider)->openOutputFile( CONST_STRING(m_serviceProvider, user), m_accountsPath );

		if( file == 0 )
		{
			LOGGER_ERROR(m_serviceProvider)( "AccountManager::saveAccountsInfo can't open file for writing. Accounts '%s' settings not saved"
				, m_accountsPath.c_str()
				);

			return false;
		}

		ConfigFile config;

		//Utils::stringWriteU( file, L"[SETTINGS]\n" );

		if( m_defaultAccountID.empty() == false )
		{
			config.setSetting( L"SETTINGS", L"DefaultAccountID", m_defaultAccountID );
			//Utils::stringWriteU( file, L"DefaultAccountID = " + m_defaultAccountID + L"\n" );
		}

		if( m_currentAccount != NULL )
		{
			const WString & name = m_currentAccount->getName();

			config.setSetting( L"SETTINGS", L"SelectAccountID", name );
			//Utils::stringWriteU( file, L"SelectAccountID = " + name + L"\n" );
		}

		//Utils::stringWriteU( file, L"AccountCount = " + Utils::toString( ( unsigned int )m_accounts.size() ) + "\n" );

        WString AccountEnumerator;
        Utils::unsignedToWString( m_playerEnumerator, AccountEnumerator );

		config.setSetting( L"SETTINGS", L"AccountEnumerator", AccountEnumerator );
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

		if( config.save( file ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "AccountManager::saveAccountsInfo '%s' settings not saved (config failed save)"
                , m_accountsPath.c_str()
                );

            file->destroy();

            return false;
        }

		file->destroy();

        return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool AccountManager::saveAccount( const WString& _accountID )
	{
		TMapAccounts::iterator it_find = m_accounts.find( _accountID );

		if( it_find == m_accounts.end() )
		{
			LOGGER_ERROR(m_serviceProvider)( "AccountManager::saveAccount: Account with ID '%ls' does not exist. Can't save"
				, _accountID.c_str() 
				);

			return false;
		}
	
        AccountInterface * account = it_find->second;

		if( account->save() == false )
        {
            LOGGER_ERROR(m_serviceProvider)( "AccountManager::saveAccount: Account with ID '%ls' does not save"
                , _accountID.c_str() 
                );

            return false;
        }

        return true;
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