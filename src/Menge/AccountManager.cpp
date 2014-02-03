#	include "AccountManager.h"
#	include "Account.h"

#   include "Interface/UnicodeInterface.h"
#   include "Interface/FileSystemInterface.h"
#   include "Interface/StringizeInterface.h"

#	include "Logger/Logger.h"

#	include "Consts.h"

#	include "Core/IniUtil.h"

#	include "Core/String.h"
#	include "Core/File.h"
#	include "Core/UnicodeFormat.h"

#	include "Config/Typedef.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( AccountService, Menge::AccountServiceInterface, Menge::AccountManager );
//////////////////////////////////////////////////////////////////////////
namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	AccountManager::AccountManager()
		: m_serviceProvider(nullptr)
        , m_accountListener(nullptr)
		, m_currentAccount(nullptr)
		, m_playerEnumerator(0)
		, m_projectVersion(0)
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
    bool AccountManager::initialize( const FilePath & _accountsPath, size_t _projectVersion, AccountServiceListener * _listener )
    {
        m_accountsPath = _accountsPath;
		m_projectVersion = _projectVersion;
        m_accountListener = _listener;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AccountManager::finalize()
    {
        LOGGER_WARNING(m_serviceProvider)("AccountManager::finalize save accounts"
            );

        AccountInterface * lastAccount = m_currentAccount;
        this->unselectCurrentAccount_();

        m_currentAccount = lastAccount;

        this->saveAccounts();

        m_currentAccount = nullptr;

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
        size_t new_playerID = ++m_playerEnumerator;

		WString accountID = (UnicodeFormat(L"Player_%d") % new_playerID).str();
        
		Account * account = this->createAccount_( accountID );

        if( account == nullptr )
        {
            return nullptr;
        }
        	
		return account;
	}
	//////////////////////////////////////////////////////////////////////////
	Account * AccountManager::createAccount_( const WString& _accountID )
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
				
		Account * newAccount = this->newAccount_( _accountID );

        if( newAccount == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("AccountManager::createAccount_: Account with ID '%ls' invalid create. Account not created"
                , _accountID.c_str() 
                );

            return nullptr;
        }
        
		m_currentAccount = newAccount;

        if( m_accountListener )
        {
		    m_accountListener->onCreateAccount( _accountID );
        }
        
        m_accounts.insert( std::make_pair( _accountID, newAccount ) );

        return newAccount;
	}
    //////////////////////////////////////////////////////////////////////////
    void AccountManager::unselectCurrentAccount_()
    {
        if( m_currentAccount == nullptr )
        {
            return;
        }
        
        if( m_accountListener != nullptr )
        {
            const WString & name = m_currentAccount->getName();
            m_accountListener->onUnselectAccount( name );
        }        

        m_currentAccount = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    Account * AccountManager::newAccount_( const WString& _accountID )
    {
        String utf8_path;
        if( Helper::unicodeToUtf8( m_serviceProvider, _accountID, utf8_path ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("AccountManager::newAccount_ can't convert accountID '%ls' to utf8"
                , _accountID.c_str() 
                );

            return nullptr;
        }

        FilePath folder = Helper::stringizeString( m_serviceProvider, utf8_path );

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

        Account * newAccount = new Account(m_serviceProvider, _accountID, m_projectVersion);

        newAccount->setFolder( folder );

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

        AccountInterface * account = it_find->second;

		if( m_currentAccount )
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

		delete static_cast<Account *>(account);

		m_accounts.erase( it_find );

        if( m_accountListener != nullptr )
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
            //else
            //{
                //LOGGER_WARNING(m_serviceProvider)( "AccountManager::selectAccount account '%ls' is alredy selected!"
                //    , _accountID.c_str() 
                //    );

                //return true;
            //}            
        }

        AccountInterface * account = it_find->second;
		
        //if( account->load() == false )
        //{
        //    LOGGER_ERROR(m_serviceProvider)("AccountManager::selectAccount Can't select account '%ls'. account not load"
        //        , _accountID.c_str() 
        //        );

        //    return false;
        //}
        
        m_currentAccount = account;

		m_currentAccount->apply();

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
			LOGGER_ERROR(m_serviceProvider)("AccountManager::getAccount account with ID '%ls' not found"
				, _accountID.c_str() 
				);

			return nullptr;
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
		Account * account = this->newAccount_( _accountID );

        if( account == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("AccountManager::loadAccount_ invalid create account %ls"
                , _accountID.c_str()
                );

            return nullptr;
        }
        
        if( m_accountListener != nullptr )
        {
            m_currentAccount = account;
            m_accountListener->onCreateAccount( _accountID );
            m_currentAccount = nullptr;
        }

        if( account->load() == false )
        {
		    delete account;

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
		bool accountsExist = FILE_SERVICE(m_serviceProvider)
            ->existFile( CONST_STRING(m_serviceProvider, user), m_accountsPath, nullptr, 0, nullptr );

		if( accountsExist == false )
		{
			LOGGER_WARNING(m_serviceProvider)( "AccountManager::loadAccounts not exist accounts '%s'"
				, m_accountsPath.c_str()
				);

			return true;
		}
        
		InputStreamInterfacePtr file = FILE_SERVICE(m_serviceProvider)
            ->openInputFile( CONST_STRING(m_serviceProvider, user), m_accountsPath );

        if( file == nullptr )
        {
            LOGGER_ERROR(m_serviceProvider)("AccountManager::loadAccounts open accounts failed '%s'"
                , m_accountsPath.c_str()
                );

            return false;
        }
		
		IniUtil::IniStore ini;
		if( IniUtil::loadIni( ini, file, m_serviceProvider ) == false )
		{
			LOGGER_ERROR(m_serviceProvider)("AccountManager::loadAccounts parsing accounts failed '%s'"
				, m_accountsPath.c_str()
				);

			return false;
		}

		file = nullptr;

		//unsigned int playerCount;

		//config.getSettingUInt( L"SETTINGS", L"AccountCount", playerCount );
        if( IniUtil::getIniValue( ini, "SETTINGS", "AccountEnumerator", m_playerEnumerator, m_serviceProvider ) == false )
        {
            LOGGER_ERROR(m_serviceProvider)("AccountManager::loadAccounts get AccountEnumerator failed '%s'"
                , m_accountsPath.c_str()
                );

            return false;
        }

		if( IniUtil::getIniValue( ini, "SETTINGS", "DefaultAccountID", m_defaultAccountID, m_serviceProvider ) == false )
        {
            LOGGER_INFO(m_serviceProvider)( "AccountManager::loadAccounts get DefaultAccountID failed '%s'"
                , m_accountsPath.c_str()
                );
        }           

		WString selectAccountID;
		if( IniUtil::getIniValue( ini, "SETTINGS", "SelectAccountID", selectAccountID, m_serviceProvider ) == false )
        {
            LOGGER_INFO(m_serviceProvider)( "AccountManager::loadAccounts get SelectAccountID failed '%s'"
                , m_accountsPath.c_str()
                );
        }   
        
        TVectorWString values;
		if( IniUtil::getIniValue( ini, "ACCOUNTS", "Account", values, m_serviceProvider ) == false )
        {
            LOGGER_INFO(m_serviceProvider)( "AccountManager::loadAccounts get ACCOUNTS failed '%s'"
                , m_accountsPath.c_str()
                );
        }  

        Account * validAccount = nullptr;

		for( TVectorWString::const_iterator
			it = values.begin(), 
			it_end = values.end();
		it != it_end;
		++it )
		{
			const WString & name = *it;

			Account * account = this->loadAccount_( name );

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
            LOGGER_ERROR(m_serviceProvider)("AccountManager::loadAccounts invalid set any accounts"
                );

            return false;
        }
		
		return true;
	}
	//////////////////////////////////////////////////////////////////////////
	bool AccountManager::saveAccounts()
	{
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
            IniUtil::writeIniSetting( m_serviceProvider, file, "Account", it->first );
		}

        for( TMapAccounts::iterator 
            it = m_accounts.begin(), 
            it_end = m_accounts.end();
        it != it_end;
        ++it )
        {
            AccountInterface * account = it->second;

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