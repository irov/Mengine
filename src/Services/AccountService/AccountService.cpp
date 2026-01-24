#include "AccountService.h"

#include "Interface/ApplicationInterface.h"
#include "Interface/UnicodeSystemInterface.h"
#include "Interface/ConfigServiceInterface.h"
#include "Interface/FileServiceInterface.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/Assertion.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/Logger.h"
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
#include "Kernel/ContentHelper.h"
#include "Kernel/VocabularyHelper.h"

#include "Config/StdString.h"

#ifndef MENGINE_ACCOUNTS_SETTINGS_JSON_PATH
#define MENGINE_ACCOUNTS_SETTINGS_JSON_PATH "accounts.json"
#endif

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
        m_factoryAccounts = Helper::makeFactoryPool<Account, 8>( MENGINE_DOCUMENT_FACTORABLE );

        FileGroupInterfacePtr fileGroup = FILE_SERVICE()
            ->getFileGroup( STRINGIZE_STRING_LOCAL( "user" ) );

        MENGINE_ASSERTION_MEMORY_PANIC( fileGroup, "not found file group 'user'" );

        m_fileGroup = fileGroup;

        ArchivatorInterfacePtr archivator = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Archivator" ), STRINGIZE_STRING_LOCAL( "lz4" ) );

        MENGINE_ASSERTION_MEMORY_PANIC( archivator, "not found 'lz4' archivator" );

        m_archivator = archivator;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AccountService::_finalizeService()
    {
        m_accountProvider = nullptr;

        m_fileGroup = nullptr;
        m_archivator = nullptr;

        m_currentAccountId.clear();

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
        ConstString lastAccount = m_currentAccountId;
        this->unselectCurrentAccount_();

        m_currentAccountId = lastAccount;

        m_invalidateAccounts = true;

        this->saveAccounts();
    }
    //////////////////////////////////////////////////////////////////////////
    void AccountService::setAccountProvider( const AccountProviderInterfacePtr & _accountProvider )
    {
        m_accountProvider = _accountProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    AccountInterfacePtr AccountService::createAccount( const DocumentInterfacePtr & _doc )
    {
        uint32_t new_playerId = ++m_playerEnumerator;

        Stringstream streamAccountId;
        streamAccountId << "Player_" << new_playerId;

        ConstString accountId = Helper::stringizeString( streamAccountId.str() );

        AccountInterfacePtr account = this->createAccount_( accountId, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( account, "account with ID '%s' invalid create. Account not created"
            , accountId.c_str()
        );

        LOGGER_INFO( "account", "create account '%s' UID '%.20s'"
            , account->getAccountId().c_str()
            , account->getUID().data
        );

        return account;
    }
    //////////////////////////////////////////////////////////////////////////
    AccountInterfacePtr AccountService::createGlobalAccount( const DocumentInterfacePtr & _doc )
    {
        uint32_t new_playerId = ++m_playerEnumerator;

        Stringstream streamAccountId;

        streamAccountId << "Global_" << new_playerId;

        ConstString accountId = Helper::stringizeString( streamAccountId.str() );

        AccountInterfacePtr account = this->createGlobalAccount_( accountId, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( account, "account with ID '%s' invalid create. Account not created"
            , accountId.c_str()
        );

        LOGGER_INFO( "account", "create account '%s' UID '%.20s'"
            , account->getAccountId().c_str()
            , account->getUID().data
        );

        return account;
    }
    //////////////////////////////////////////////////////////////////////////
    AccountInterfacePtr AccountService::createAccount_( const ConstString & _accountId, const DocumentInterfacePtr & _doc )
    {
        MENGINE_ASSERTION_FATAL( m_accounts.find( _accountId ) == nullptr, "account with ID '%s' already exist. Account not created"
            , _accountId.c_str()
        );

        this->unselectCurrentAccount_();

        AccountInterfacePtr newAccount = this->newAccount_( _accountId, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( newAccount, "account with ID '%s' invalid create. Account not created"
            , _accountId.c_str()
        );

        AccountUID uid;
        Helper::makeUID( 20, uid.data );

        newAccount->setUID( uid );

        m_accounts.emplace( _accountId, newAccount );

        m_currentAccountId = _accountId;        

        if( m_accountProvider != nullptr )
        {
            m_accountProvider->onCreateAccount( m_currentAccountId, false );
        }

        newAccount->apply();

        if( m_accountProvider != nullptr )
        {
            m_accountProvider->onSelectAccount( m_currentAccountId );
        }

        m_invalidateAccounts = true;

        return newAccount;
    }
    //////////////////////////////////////////////////////////////////////////
    AccountInterfacePtr AccountService::createGlobalAccount_( const ConstString & _accountId, const DocumentInterfacePtr & _doc )
    {
#if defined(MENGINE_DEBUG)
        AccountInterfacePtr account = m_accounts.find( _accountId );

        if( account != nullptr )
        {
            LOGGER_ERROR( "account with ID '%s' already exist. Account not created"
                , _accountId.c_str()
            );

            return nullptr;
        }
#endif

        AccountInterfacePtr newAccount = this->newAccount_( _accountId, _doc );

        MENGINE_ASSERTION_MEMORY_PANIC( newAccount, "Account with ID '%s' invalid create. Account not created"
            , _accountId.c_str()
        );

        AccountUID uid;
        Helper::makeUID( 20, uid.data );

        newAccount->setUID( uid );

        m_accounts.emplace( _accountId, newAccount );

        m_globalAccountId = _accountId;

        if( m_accountProvider != nullptr )
        {
            m_accountProvider->onCreateAccount( m_globalAccountId, true );
        }

        newAccount->apply();

        m_invalidateAccounts = true;

        return newAccount;
    }
    //////////////////////////////////////////////////////////////////////////
    void AccountService::unselectCurrentAccount_()
    {
        if( m_currentAccountId.empty() == true )
        {
            return;
        }

        ConstString currentAccount = m_currentAccountId;

        if( m_accountProvider != nullptr )
        {
            m_accountProvider->onUnselectAccount( currentAccount );
        }

        if( m_currentAccountId == currentAccount )
        {
            m_currentAccountId.clear();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    AccountInterfacePtr AccountService::newAccount_( const ConstString & _accountId, const DocumentInterfacePtr & _doc )
    {
        PathString accountString;
        accountString.append( _accountId.c_str(), _accountId.size() );
        accountString.append( MENGINE_PATH_FORWARDSLASH );

        FilePath accountPath = Helper::stringizeFilePath( accountString );

        if( m_fileGroup->createDirectory( accountPath ) == false )
        {
            LOGGER_ERROR( "account with ID '%s' failed create directory '%s'"
                , _accountId.c_str()
                , accountPath.c_str()
            );

            return nullptr;
        }

        AccountPtr newAccount = m_factoryAccounts->createObject( _doc );

        uint32_t projectVersion = APPLICATION_SERVICE()
            ->getProjectVersion();

        if( newAccount->initialize( _accountId, m_archivator, m_fileGroup, accountPath, projectVersion ) == false )
        {
            LOGGER_ERROR( "account with ID '%s' invalid initialize"
                , _accountId.c_str()
            );

            return nullptr;
        }

        m_invalidateAccounts = true;

        return newAccount;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AccountService::hasAccount( const ConstString & _accountId ) const
    {
        bool exist = m_accounts.exist( _accountId );

        return exist;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AccountService::deleteAccount( const ConstString & _accountId )
    {
        if( m_currentAccountId.empty() == false )
        {
            if( m_currentAccountId == _accountId )
            {
                this->unselectCurrentAccount_();
            }
        }

        if( m_accountProvider != nullptr )
        {
            m_accountProvider->onDeleteAccount( _accountId );
        }

        AccountPtr account = m_accounts.erase( _accountId );

        if( account == nullptr )
        {
            LOGGER_ERROR( "can't delete account '%s'. There is no account with such ID"
                , _accountId.c_str()
            );

            return false;
        }

        LOGGER_INFO( "account", "delete account '%s' UID '%.20s'"
            , account->getAccountId().c_str()
            , account->getUID().data
        );

        account->finalize();

        m_invalidateAccounts = true;

        this->saveAccounts();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AccountService::selectAccount( const ConstString & _accountId )
    {
        const AccountInterfacePtr & account = m_accounts.find( _accountId );

        if( account == nullptr )
        {
            LOGGER_ERROR( "can't select account '%s'. There is no account with such ID"
                , _accountId.c_str()
            );

            return false;
        }

        if( m_currentAccountId.empty() == false )
        {
            if( m_currentAccountId != _accountId )
            {
                this->unselectCurrentAccount_();
            }
        }

        m_currentAccountId = _accountId;

        LOGGER_INFO( "account", "select account '%s' UID '%.20s'"
            , account->getAccountId().c_str()
            , account->getUID().data
        );

        account->apply();

        if( m_accountProvider != nullptr )
        {
            m_accountProvider->onSelectAccount( _accountId );
        }

        m_invalidateAccounts = true;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AccountService::hasCurrentAccount() const
    {
        return m_currentAccountId.empty() == false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AccountService::deleteCurrentAccount()
    {
        if( m_currentAccountId.empty() == true )
        {
            LOGGER_ERROR( "can't delete current account. Current account not selected" );

            return false;
        }

        ConstString deleteAccountId = m_currentAccountId;

        if( this->deleteAccount( deleteAccountId ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & AccountService::getCurrentAccountId() const
    {
        return m_currentAccountId;
    }
    //////////////////////////////////////////////////////////////////////////
    const AccountInterfacePtr & AccountService::getCurrentAccount() const
    {
        if( m_currentAccountId.empty() == true )
        {
            return AccountInterfacePtr::none();
        }

        const AccountInterfacePtr & account = m_accounts.find( m_currentAccountId );

        MENGINE_ASSERTION_MEMORY_PANIC( account, "current account with ID '%s' not found"
            , m_currentAccountId.c_str()
        );

        return account;
    }
    //////////////////////////////////////////////////////////////////////////
    const AccountInterfacePtr & AccountService::getAccount( const ConstString & _accountId ) const
    {
        const AccountInterfacePtr & account = m_accounts.find( _accountId );

        MENGINE_ASSERTION_MEMORY_PANIC( account, "account with ID '%s' not found"
            , _accountId.c_str()
        );

        return account;
    }
    //////////////////////////////////////////////////////////////////////////
    void AccountService::foreachAccounts( const LambdaAccounts & _lambda ) const
    {
        for( const HashtableAccounts::value_type & value : m_accounts )
        {
            const AccountPtr & account = value.element;

            const ConstString & accountId = account->getAccountId();

            if( accountId == m_globalAccountId )
            {
                continue;
            }

            _lambda( account );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void AccountService::setDefaultAccount( const ConstString & _accountId )
    {
        m_defaultAccountId = _accountId;

        m_invalidateAccounts = true;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & AccountService::getDefaultAccountId() const
    {
        return m_defaultAccountId;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AccountService::isCurrentDefaultAccount() const
    {
        if( m_defaultAccountId.empty() == true )
        {
            return false;
        }

        if( m_currentAccountId.empty() == true )
        {
            return false;
        }

        return m_defaultAccountId == m_currentAccountId;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AccountService::hasDefaultAccount() const
    {
        if( m_defaultAccountId.empty() == true )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AccountService::setGlobalAccount( const ConstString & _accountId )
    {
        LOGGER_INFO( "account", "set global account '%s'"
            , _accountId.c_str()
        );

        m_globalAccountId = _accountId;

        m_invalidateAccounts = true;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & AccountService::getGlobalAccountId() const
    {
        return m_globalAccountId;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AccountService::hasGlobalAccount() const
    {
        if( m_globalAccountId.empty() == true )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AccountService::selectDefaultAccount()
    {
        if( m_defaultAccountId.empty() == true )
        {
            return false;
        }

        if( this->selectAccount( m_defaultAccountId ) == false )
        {
            LOGGER_ERROR( "invalid select account '%s'"
                , m_defaultAccountId.c_str()
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AccountService::loadAccount_( const AccountInterfacePtr & _account )
    {
        const ConstString & accountId = _account->getAccountId();

        if( m_accountProvider != nullptr )
        {
            m_currentAccountId = accountId;
            m_accountProvider->onCreateAccount( accountId, m_globalAccountId == accountId );
            m_currentAccountId.clear();
        }

        if( _account->load() == false )
        {
            LOGGER_ERROR( "invalid load account '%s'"
                , accountId.c_str()
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AccountService::loadAccounts()
    {
        bool OPTION_noaccounts = HAS_OPTION( "noaccounts" );

        if( OPTION_noaccounts == true )
        {
            return true;
        }

        ConfigInterfacePtr config = this->getLoadSettingConfig_();

        if( config == nullptr )
        {
            return true;
        }

        if( config->hasValueInteger( "SETTINGS", "AccountEnumerator", MENGINE_UINT32_C(0), &m_playerEnumerator ) == false )
        {
            LOGGER_ERROR( "get [SETTINGS] AccountEnumerator failed" );

            return false;
        }

        if( config->hasValue( "SETTINGS", "GlobalAccountID", ConstString::none(), &m_globalAccountId ) == false )
        {
            LOGGER_ERROR( "get [SETTINGS] GlobalAccountID failed" );
        }

        if( config->hasValue( "SETTINGS", "DefaultAccountID", ConstString::none(), &m_defaultAccountId ) == false )
        {
            LOGGER_ERROR( "get [SETTINGS] DefaultAccountID failed" );
        }

        ConstString selectAccountId;
        if( config->hasValue( "SETTINGS", "SelectAccountID", ConstString::none(), &selectAccountId ) == false )
        {
            LOGGER_ERROR( "get [SETTINGS] SelectAccountID failed" );
        }

        VectorConstString values;
        config->getValues( "ACCOUNTS", "Account", &values );

        AccountInterfacePtr validAccount = nullptr;

        for( const ConstString & accountId : values )
        {
            AccountInterfacePtr account = this->newAccount_( accountId, MENGINE_DOCUMENT_FACTORABLE );

            MENGINE_ASSERTION_MEMORY_PANIC( account, "invalid create account '%s'"
                , accountId.c_str()
            );

            m_accounts.emplace( accountId, account );

            if( this->loadAccount_( account ) == false )
            {
                m_accounts.erase( accountId );

                LOGGER_ERROR( "invalid load account '%s'"
                    , accountId.c_str()
                );

                continue;
            }

            if( accountId != m_globalAccountId )
            {
                validAccount = account;
            }
        }

        if( this->hasAccount( selectAccountId ) == false )
        {
            selectAccountId.clear();
        }

        if( this->hasAccount( m_defaultAccountId ) == false )
        {
            m_defaultAccountId.clear();
        }

        if( this->hasAccount( m_globalAccountId ) == false )
        {
            m_globalAccountId.clear();
        }

        if( selectAccountId.empty() == false )
        {
            LOGGER_INFO( "account", "select account '%s'"
                , selectAccountId.c_str()
            );

            if( this->selectAccount( selectAccountId ) == false )
            {
                LOGGER_ERROR( "invalid set select account '%s'"
                    , selectAccountId.c_str()
                );

                return false;
            }
        }
        else if( m_defaultAccountId.empty() == false )
        {
            LOGGER_INFO( "account", "set default account '%s'"
                , m_defaultAccountId.c_str()
            );

            if( this->selectAccount( m_defaultAccountId ) == false )
            {
                LOGGER_ERROR( "invalid set default account '%s'"
                    , m_defaultAccountId.c_str()
                );

                return false;
            }
        }
        else if( validAccount != nullptr )
        {
            const ConstString & accountId = validAccount->getAccountId();

            LOGGER_INFO( "account", "set valid account '%s'"
                , accountId.c_str()
            );

            if( this->selectAccount( accountId ) == false )
            {
                LOGGER_ERROR( "invalid set valid account '%s'"
                    , accountId.c_str()
                );

                return false;
            }
        }
        else
        {
            LOGGER_WARNING( "invalid set any accounts" );
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
            ContentInterfacePtr settingsJSONContent = Helper::makeFileContent( m_fileGroup, settingsJSONPath, MENGINE_DOCUMENT_FACTORABLE );

            ConfigInterfacePtr config = Helper::loadConfig( settingsJSONContent, ConstString::none(), MENGINE_DOCUMENT_FACTORABLE );

            if( config == nullptr )
            {
                LOGGER_ERROR( "parsing accounts failed '%s'"
                    , Helper::getContentFullPath( settingsJSONContent ).c_str()
                );

                return nullptr;
            }

            return config;
        }

        FilePath settingsINIPath = Helper::stringizeFilePath( MENGINE_ACCOUNTS_SETTINGS_INI_PATH );

        if( m_fileGroup->existFile( settingsINIPath, true ) == true )
        {
            ContentInterfacePtr settingsINIContent = Helper::makeFileContent( m_fileGroup, settingsINIPath, MENGINE_DOCUMENT_FACTORABLE );

            ConfigInterfacePtr config = Helper::loadConfig( settingsINIContent, ConstString::none(), MENGINE_DOCUMENT_FACTORABLE );

            if( config == nullptr )
            {
                LOGGER_ERROR( "parsing accounts failed '%s'"
                    , Helper::getContentFullPath( settingsINIContent ).c_str()
                );

                return nullptr;
            }

            return config;
        }

        LOGGER_MESSAGE( "not exist accounts '%s' or '%s'"
            , settingsJSONPath.c_str()
            , settingsINIPath.c_str()
        );

        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AccountService::saveAccounts()
    {
        LOGGER_INFO( "account", "save accounts" );

        if( m_invalidateAccounts == false )
        {
            return true;
        }

        FilePath Game_AccountsPath = CONFIG_VALUE_FILEPATH( "Game", "AccountsPath", STRINGIZE_FILEPATH_LOCAL_I( MENGINE_ACCOUNTS_SETTINGS_JSON_PATH ) );

        OutputStreamInterfacePtr stream = Helper::openOutputStreamFile( m_fileGroup, Game_AccountsPath, true, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( stream, "can't open file for writing. Accounts '%s' settings not saved"
            , Game_AccountsPath.c_str()
        );

        jpp::object j_root = jpp::make_object();

        jpp::object j_settings = jpp::make_object();

        if( m_globalAccountId.empty() == false )
        {
            j_settings.set( "GlobalAccountID", m_globalAccountId );
        }

        if( m_defaultAccountId.empty() == false )
        {
            j_settings.set( "DefaultAccountID", m_defaultAccountId );
        }

        if( m_currentAccountId.empty() == false )
        {
            j_settings.set( "SelectAccountID", m_currentAccountId );
        }

        j_settings.set( "AccountEnumerator", m_playerEnumerator );

        jpp::object j_accounts = jpp::make_object();

        jpp::array j_list_account = jpp::make_array();
        for( const HashtableAccounts::value_type & value : m_accounts )
        {
            const ConstString & accountId = value.key;
            
            j_list_account.push_back( accountId );
        }

        j_accounts.set( "Account", j_list_account );

        j_root.set( "SETTINGS", j_settings );
        j_root.set( "ACCOUNTS", j_accounts );

        if( Helper::writeJSONStream( j_root, stream ) == false )
        {
            LOGGER_ERROR( "invalid write accounts '%s'"
                , Game_AccountsPath.c_str()
            );

            return false;
        }

        if( Helper::closeOutputStreamFile( m_fileGroup, stream ) == false )
        {
            LOGGER_ERROR( "can't close file after writing. Accounts '%s' settings not saved"
                , Game_AccountsPath.c_str()
            );

            return false;
        }

        for( const HashtableAccounts::value_type & value : m_accounts )
        {
            const AccountInterfacePtr & account = value.element;

            if( account->save() == false )
            {
                LOGGER_ERROR( "invalid save account id '%s' folder '%s'"
                    , account->getAccountId().c_str()
                    , account->getFolderName().c_str()
                );

                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
