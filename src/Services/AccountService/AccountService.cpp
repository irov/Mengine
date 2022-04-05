#include "AccountService.h"

#include "Interface/ApplicationInterface.h"
#include "Interface/UnicodeSystemInterface.h"
#include "Interface/ConfigServiceInterface.h"
#include "Interface/VocabularyServiceInterface.h"
#include "Interface/FileServiceInterface.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/Assertion.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/IniHelper.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/FileStreamHelper.h"
#include "Kernel/StringHelper.h"
#include "Kernel/Stringstream.h"
#include "Kernel/PathString.h"
#include "Kernel/ConfigHelper.h"
#include "Kernel/UID.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/NotificationHelper.h"
#include "Kernel/JSONHelper.h"

#include "Config/StdString.h"

//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_ACCOUNTS_SETTINGS_JSON_PATH
#define MENGINE_ACCOUNTS_SETTINGS_JSON_PATH "accounts.json"
#endif
//////////////////////////////////////////////////////////////////////////
#ifndef MENGINE_ACCOUNTS_SETTINGS_INI_PATH
#define MENGINE_ACCOUNTS_SETTINGS_INI_PATH "accounts.ini"
#endif
//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( AccountService, Mengine::AccountService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AccountService::AccountService()
        : m_playerEnumerator( 0 )
        , m_invalidateAccounts( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AccountService::~AccountService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AccountService::_initializeService()
    {
        LOGGER_INFO( "account", "Initializing Account manager..." );

        m_factoryAccounts = Helper::makeFactoryPool<Account, 8>( MENGINE_DOCUMENT_FACTORABLE );

        FileGroupInterfacePtr fileGroup = FILE_SERVICE()
            ->getFileGroup( STRINGIZE_STRING_LOCAL( "user" ) );

        MENGINE_ASSERTION_MEMORY_PANIC( fileGroup );

        m_fileGroup = fileGroup;

        ArchivatorInterfacePtr archivator = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Archivator" ), STRINGIZE_STRING_LOCAL( "lz4" ) );

        MENGINE_ASSERTION_MEMORY_PANIC( archivator );

        m_archivator = archivator;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AccountService::_finalizeService()
    {
        m_accountProvider = nullptr;

        m_fileGroup = nullptr;
        m_archivator = nullptr;

        m_currentAccountID.clear();

        for( const HashtableAccounts::value_type & value : m_accounts )
        {
            const AccountPtr & account = value.element;

            account->finalize();
        }

        m_accounts.clear();

        MENGINE_ASSERTION_FACTORY_EMPTY( m_factoryAccounts );

        m_factoryAccounts = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void AccountService::stopAccounts()
    {
        ConstString lastAccount = m_currentAccountID;
        this->unselectCurrentAccount_();

        m_currentAccountID = lastAccount;

        this->saveAccounts();
    }
    //////////////////////////////////////////////////////////////////////////
    void AccountService::setAccountProvider( const AccountProviderInterfacePtr & _accountProvider )
    {
        m_accountProvider = _accountProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    AccountInterfacePtr AccountService::createAccount( const DocumentPtr & _doc )
    {
        uint32_t new_playerID = ++m_playerEnumerator;

        Stringstream streamAccountID;
        streamAccountID << "Player_" << new_playerID;

        ConstString accountID = Helper::stringizeString( streamAccountID.str() );

        AccountInterfacePtr account = this->createAccount_( accountID, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( account );

        LOGGER_MESSAGE( "create account '%s' UID '%.20s'"
            , account->getID().c_str()
            , account->getUID().data
        );

        return account;
    }
    //////////////////////////////////////////////////////////////////////////
    AccountInterfacePtr AccountService::createGlobalAccount( const DocumentPtr & _doc )
    {
        uint32_t new_playerID = ++m_playerEnumerator;

        Stringstream streamAccountID;

        streamAccountID << "Global_" << new_playerID;

        ConstString accountID = Helper::stringizeString( streamAccountID.str() );

        AccountInterfacePtr account = this->createGlobalAccount_( accountID, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( account );

        LOGGER_MESSAGE( "create account '%s' UID '%.20s'"
            , account->getID().c_str()
            , account->getUID().data
        );

        return account;
    }
    //////////////////////////////////////////////////////////////////////////
    AccountInterfacePtr AccountService::createAccount_( const ConstString & _accountID, const DocumentPtr & _doc )
    {
        MENGINE_ASSERTION_FATAL( m_accounts.find( _accountID ) == nullptr, "account with ID '%s' already exist. Account not created"
            , _accountID.c_str()
        );

        this->unselectCurrentAccount_();

        AccountInterfacePtr newAccount = this->newAccount_( _accountID, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( newAccount, "account with ID '%s' invalid create. Account not created"
            , _accountID.c_str()
        );

        m_currentAccountID = newAccount->getID();

        m_accounts.emplace( _accountID, newAccount );

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
    AccountInterfacePtr AccountService::createGlobalAccount_( const ConstString & _accountID, const DocumentPtr & _doc )
    {
#ifdef MENGINE_DEBUG
        AccountInterfacePtr account = m_accounts.find( _accountID );

        if( account != nullptr )
        {
            LOGGER_ERROR( "Account with ID '%s' already exist. Account not created"
                , _accountID.c_str()
            );

            return nullptr;
        }
#endif

        AccountInterfacePtr newAccount = this->newAccount_( _accountID, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( newAccount, "Account with ID '%s' invalid create. Account not created"
            , _accountID.c_str()
        );

        m_globalAccountID = newAccount->getID();

        m_accounts.emplace( _accountID, newAccount );

        newAccount->apply();

        return newAccount;
    }
    //////////////////////////////////////////////////////////////////////////
    void AccountService::unselectCurrentAccount_()
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
    AccountInterfacePtr AccountService::newAccount_( const ConstString & _accountID, const DocumentPtr & _doc )
    {
        PathString accountString;
        accountString.append( _accountID );
        accountString.append( MENGINE_PATH_DELIM );

        FilePath accountPath = Helper::stringizeFilePath( accountString );

        if( m_fileGroup->createDirectory( accountPath ) == false )
        {
            LOGGER_ERROR( "Account '%s' failed create directory '%s'"
                , _accountID.c_str()
                , accountPath.c_str()
            );

            return nullptr;
        }

        AccountPtr newAccount = m_factoryAccounts->createObject( _doc );

        uint32_t projectVersion = APPLICATION_SERVICE()
            ->getProjectVersion();

        if( newAccount->initialize( _accountID, m_archivator, m_fileGroup, accountPath, projectVersion ) == false )
        {
            LOGGER_ERROR( "Account '%s' invalid initialize"
                , _accountID.c_str()
            );

            return nullptr;
        }

        m_invalidateAccounts = true;

        return newAccount;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AccountService::hasAccount( const ConstString & _accountID ) const
    {
        bool exist = m_accounts.exist( _accountID );

        return exist;
    }
    //////////////////////////////////////////////////////////////////////////
    void AccountService::deleteAccount( const ConstString & _accountID )
    {
        AccountPtr account = m_accounts.erase( _accountID );

        MENGINE_ASSERTION_MEMORY_PANIC( account, "can't delete account '%s'. There is no account with such ID"
            , _accountID.c_str()
        );

        if( m_currentAccountID.empty() == false )
        {
            if( m_currentAccountID == _accountID )
            {
                this->unselectCurrentAccount_();
            }
        }

        LOGGER_MESSAGE( "delete account '%s' UID '%.20s'"
            , account->getID().c_str()
            , account->getUID().data
        );

        account->finalize();

        if( m_accountProvider != nullptr )
        {
            m_accountProvider->onDeleteAccount( _accountID );
        }

        m_invalidateAccounts = true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AccountService::selectAccount( const ConstString & _accountID )
    {
        const AccountInterfacePtr & account = m_accounts.find( _accountID );

        if( account == nullptr )
        {
            LOGGER_ERROR( "can't select account '%s'. There is no account with such ID"
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

        m_currentAccountID = _accountID;

        LOGGER_MESSAGE( "select account '%s' UID '%.20s'"
            , account->getID().c_str()
            , account->getUID().data
        );

        account->apply();

        if( m_accountProvider != nullptr )
        {
            m_accountProvider->onSelectAccount( _accountID );
        }

        m_invalidateAccounts = true;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AccountService::hasCurrentAccount() const
    {
        return m_currentAccountID.empty() == false;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & AccountService::getCurrentAccountID() const
    {
        return m_currentAccountID;
    }
    //////////////////////////////////////////////////////////////////////////
    const AccountInterfacePtr & AccountService::getAccount( const ConstString & _accountID ) const
    {
        const AccountInterfacePtr & account = m_accounts.find( _accountID );

        MENGINE_ASSERTION_MEMORY_PANIC( account, "account with ID '%s' not found"
            , _accountID.c_str()
        );

        return account;
    }
    //////////////////////////////////////////////////////////////////////////
    void AccountService::foreachAccounts( const LambdaAccounts & _lambda ) const
    {
        for( const HashtableAccounts::value_type & value : m_accounts )
        {
            const AccountPtr & account = value.element;

            const ConstString & accountID = account->getID();

            if( accountID == m_globalAccountID )
            {
                continue;
            }

            _lambda( account );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void AccountService::setDefaultAccount( const ConstString & _accountID )
    {
        m_defaultAccountID = _accountID;

        m_invalidateAccounts = true;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & AccountService::getDefaultAccountID() const
    {
        return m_defaultAccountID;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AccountService::isCurrentDefaultAccount() const
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
    bool AccountService::hasDefaultAccount() const
    {
        if( m_defaultAccountID.empty() == true )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AccountService::setGlobalAccount( const ConstString & _accountID )
    {
        LOGGER_INFO( "account", "set global account '%s'"
            , _accountID.c_str()
        );

        m_globalAccountID = _accountID;

        m_invalidateAccounts = true;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & AccountService::getGlobalAccountID() const
    {
        return m_globalAccountID;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AccountService::hasGlobalAccount() const
    {
        if( m_globalAccountID.empty() == true )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AccountService::selectDefaultAccount()
    {
        if( m_defaultAccountID.empty() == true )
        {
            return false;
        }

        if( this->selectAccount( m_defaultAccountID ) == false )
        {
            LOGGER_ERROR( "invalid select account '%s'"
                , m_defaultAccountID.c_str()
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AccountService::loadAccount_( const AccountInterfacePtr & _account )
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
            LOGGER_ERROR( "invalid load account '%s'"
                , accountID.c_str()
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AccountService::loadAccounts()
    {
        bool OPTION_noLoadAccount = HAS_OPTION( "noaccounts" );

        if( OPTION_noLoadAccount == true )
        {
            return true;
        }

        ConfigInterfacePtr config = this->getLoadSettingConfig_();

        if( config == nullptr )
        {
            return true;
        }

        if( config->hasValue( "SETTINGS", "AccountEnumerator", 0u, &m_playerEnumerator ) == false )
        {
            LOGGER_ERROR( "get [SETTINGS] AccountEnumerator failed" );

            return false;
        }

        if( config->hasValue( "SETTINGS", "GlobalAccountID", ConstString::none(), &m_globalAccountID ) == false )
        {
            LOGGER_ERROR( "get [SETTINGS] GlobalAccountID failed" );
        }

        if( config->hasValue( "SETTINGS", "DefaultAccountID", ConstString::none(), &m_defaultAccountID ) == false )
        {
            LOGGER_ERROR( "get [SETTINGS] DefaultAccountID failed" );
        }

        ConstString selectAccountID;
        if( config->hasValue( "SETTINGS", "SelectAccountID", ConstString::none(), &selectAccountID ) == false )
        {
            LOGGER_ERROR( "get [SETTINGS] SelectAccountID failed" );
        }

        VectorConstString values;
        config->getValues( "ACCOUNTS", "Account", &values );

        AccountInterfacePtr validAccount = nullptr;

        for( const ConstString & accountID : values )
        {
            AccountInterfacePtr account = this->newAccount_( accountID, MENGINE_DOCUMENT_FACTORABLE );

            MENGINE_ASSERTION_MEMORY_PANIC( account, "invalid create account '%s'"
                , accountID.c_str()
            );

            m_accounts.emplace( accountID, account );

            if( this->loadAccount_( account ) == false )
            {
                m_accounts.erase( accountID );

                LOGGER_ERROR( "invalid load account '%s'"
                    , accountID.c_str()
                );

                continue;
            }

            validAccount = account;
        }

        if( this->hasAccount( selectAccountID ) == false )
        {
            selectAccountID.clear();
        }

        if( this->hasAccount( m_defaultAccountID ) == false )
        {
            m_defaultAccountID.clear();
        }

        if( this->hasAccount( m_globalAccountID ) == false )
        {
            m_globalAccountID.clear();
        }

        if( selectAccountID.empty() == false )
        {
            LOGGER_INFO( "account", "select account '%s'"
                , selectAccountID.c_str()
            );

            if( this->selectAccount( selectAccountID ) == false )
            {
                LOGGER_ERROR( "invalid set select account '%s'"
                    , selectAccountID.c_str()
                );

                return false;
            }
        }
        else if( m_defaultAccountID.empty() == false )
        {
            LOGGER_INFO( "account", "set default account '%s'"
                , m_defaultAccountID.c_str()
            );

            if( this->selectAccount( m_defaultAccountID ) == false )
            {
                LOGGER_ERROR( "invalid set default account '%s'"
                    , m_defaultAccountID.c_str()
                );

                return false;
            }
        }
        else if( validAccount != nullptr )
        {
            const ConstString & accountID = validAccount->getID();

            LOGGER_MESSAGE( "set valid account '%s'"
                , accountID.c_str()
            );

            if( this->selectAccount( accountID ) == false )
            {
                LOGGER_ERROR( "invalid set valid account '%s'"
                    , accountID.c_str()
                );

                return false;
            }
        }
        else
        {
            LOGGER_INFO( "account", "invalid set any accounts" );
        }

        m_invalidateAccounts = true;

        NOTIFICATION_NOTIFY( NOTIFICATOR_ACCOUNTS_LOAD );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    ConfigInterfacePtr AccountService::getLoadSettingConfig_() const
    {
        FilePath settingsJSONPath = Helper::stringizeFilePath( MENGINE_ACCOUNTS_SETTINGS_JSON_PATH );

        if( m_fileGroup->existFile( settingsJSONPath, true ) == true )
        {
            ConfigInterfacePtr config = CONFIG_SERVICE()
                ->loadConfig( m_fileGroup, settingsJSONPath, ConstString::none(), MENGINE_DOCUMENT_FACTORABLE );

            if( config == nullptr )
            {
                LOGGER_ERROR( "parsing accounts failed '%s'"
                    , settingsJSONPath.c_str()
                );

                return nullptr;
            }

            return config;
        }

        FilePath settingsINIPath = Helper::stringizeFilePath( MENGINE_ACCOUNTS_SETTINGS_INI_PATH );

        if( m_fileGroup->existFile( settingsINIPath, true ) == true )
        {
            ConfigInterfacePtr config = CONFIG_SERVICE()
                ->loadConfig( m_fileGroup, settingsINIPath, ConstString::none(), MENGINE_DOCUMENT_FACTORABLE );

            if( config == nullptr )
            {
                LOGGER_ERROR( "parsing accounts failed '%s'"
                    , settingsINIPath.c_str()
                );

                return nullptr;
            }

            return config;
        }

        LOGGER_WARNING( "not exist accounts '%s' or '%s'"
            , settingsJSONPath.c_str()
            , settingsINIPath.c_str()
        );

        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AccountService::saveAccounts()
    {
        LOGGER_MESSAGE( "save accounts" );

        if( m_invalidateAccounts == false )
        {
            return true;
        }

        FilePath Game_AccountsPath = CONFIG_VALUE( "Game", "AccountsPath", Helper::stringizeFilePath( MENGINE_ACCOUNTS_SETTINGS_JSON_PATH ) );

        OutputStreamInterfacePtr file = Helper::openOutputStreamFile( m_fileGroup, Game_AccountsPath, true, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( file, "can't open file for writing. Accounts '%s' settings not saved"
            , Game_AccountsPath.c_str()
        );

        jpp::object j_root = jpp::make_object();

        jpp::object j_settings = jpp::make_object();

        if( m_globalAccountID.empty() == false )
        {
            j_settings.set( "GlobalAccountID", m_globalAccountID.c_str() );
        }

        if( m_defaultAccountID.empty() == false )
        {
            j_settings.set( "DefaultAccountID", m_defaultAccountID.c_str() );
        }

        if( m_currentAccountID.empty() == false )
        {
            j_settings.set( "SelectAccountID", m_currentAccountID.c_str() );
        }

        j_settings.set( "AccountEnumerator", m_playerEnumerator );

        jpp::object j_accounts = jpp::make_object();

        jpp::array j_list_account = jpp::make_array();
        for( const HashtableAccounts::value_type & value : m_accounts )
        {
            const ConstString & accountID = value.key;
            
            j_list_account.push_back( accountID.c_str() );
        }

        j_accounts.set( "Account", j_list_account );

        j_root.set( "SETTINGS", j_settings );
        j_root.set( "ACCOUNTS", j_accounts );

        if( Helper::writeJSONStream( j_root, file ) == false )
        {
            return false;
        }

        if( m_fileGroup->closeOutputFile( file ) == false )
        {
            return false;
        }

        for( const HashtableAccounts::value_type & value : m_accounts )
        {
            const AccountInterfacePtr & account = value.element;

            if( account->save() == false )
            {
                LOGGER_ERROR( "invalid save account id '%s' folder '%s'"
                    , account->getID().c_str()
                    , account->getFolder().c_str()
                );

                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}