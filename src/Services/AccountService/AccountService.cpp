#include "AccountService.h"
#include "Account.h"

#include "Interface/ApplicationInterface.h"
#include "Interface/OptionsInterface.h"
#include "Interface/UnicodeSystemInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/StringizeServiceInterface.h"
#include "Interface/ConfigServiceInterface.h"
#include "Interface/VocabularyServiceInterface.h"

#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"

#include "Kernel/Logger.h"
#include "Kernel/Document.h"

#include "Kernel/IniUtil.h"
#include "Kernel/String.h"
#include "Kernel/UID.h"

#include "Config/Typedef.h"
#include "Config/Stringstream.h"

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
        LOGGER_INFO( "Initializing Account manager..."
        );

        m_factoryAccounts = new FactoryPool<Account, 8>();

        FileGroupInterfacePtr fileGroup = FILE_SERVICE()
            ->getFileGroup( STRINGIZE_STRING_LOCAL( "user" ) );

        if( fileGroup == nullptr )
        {
            return false;
        }

        m_fileGroup = fileGroup;

        ArchivatorInterfacePtr archivator = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "Archivator" ), STRINGIZE_STRING_LOCAL( "lz4" ) );

        if( archivator == nullptr )
        {
            return false;
        }

        m_archivator = archivator;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AccountService::_finalizeService()
    {
        LOGGER_MESSAGE( "save accounts" );

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
    void AccountService::setAccountProvider( const AccountProviderInterfacePtr & _accountProvider )
    {
        m_accountProvider = _accountProvider;
    }
    //////////////////////////////////////////////////////////////////////////
    AccountInterfacePtr AccountService::createAccount()
    {
        uint32_t new_playerID = ++m_playerEnumerator;

        Stringstream streamAccountID;
        streamAccountID << "Player_" << new_playerID;

        ConstString accountID = Helper::stringizeString( streamAccountID.str() );

        AccountInterfacePtr account = this->createAccount_( accountID );

        AccountUID uid;
        Helper::makeUID( 20, uid.data );

        account->setUID( uid );

        if( account == nullptr )
        {
            return nullptr;
        }

        return account;
    }
    //////////////////////////////////////////////////////////////////////////
    AccountInterfacePtr AccountService::createGlobalAccount()
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
    AccountInterfacePtr AccountService::createAccount_( const ConstString& _accountID )
    {
        MapAccounts::iterator it_find = m_accounts.find( _accountID );

        if( it_find != m_accounts.end() )
        {
            LOGGER_ERROR( "Account with ID '%s' already exist. Account not created"
                , _accountID.c_str()
            );

            return nullptr;
        }

        this->unselectCurrentAccount_();

        AccountInterfacePtr newAccount = this->newAccount_( _accountID );

        if( newAccount == nullptr )
        {
            LOGGER_ERROR( "Account with ID '%s' invalid create. Account not created"
                , _accountID.c_str()
            );

            return nullptr;
        }

        AccountUID uid;
        Helper::makeUID( 20, uid.data );

        newAccount->setUID( uid );

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
    AccountInterfacePtr AccountService::createGlobalAccount_( const ConstString& _accountID )
    {
        MapAccounts::iterator it_find = m_accounts.find( _accountID );

        if( it_find != m_accounts.end() )
        {
            LOGGER_ERROR( "Account with ID '%s' already exist. Account not created"
                , _accountID.c_str()
            );

            return nullptr;
        }

        AccountInterfacePtr newAccount = this->newAccount_( _accountID );

        if( newAccount == nullptr )
        {
            LOGGER_ERROR( "Account with ID '%s' invalid create. Account not created"
                , _accountID.c_str()
            );

            return nullptr;
        }

        AccountUID uid;
        Helper::makeUID( 20, uid.data );

        newAccount->setUID( uid );

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
    AccountInterfacePtr AccountService::newAccount_( const ConstString& _accountID )
    {
        PathString accountString;
        accountString.append( _accountID );
        accountString.append( '/' );

        FilePath accountPath = Helper::stringizeFilePath( accountString );

        if( m_fileGroup->createDirectory( accountPath ) == false )
        {
            LOGGER_ERROR( "Account '%s' failed create directory"
                , accountPath.c_str()
            );

            return nullptr;
        }

        AccountPtr newAccount = m_factoryAccounts->createObject( MENGINE_DOCUMENT_FUNCTION );

        uint32_t projectVersion = APPLICATION_SERVICE()
            ->getProjectVersion();

        if( newAccount->initialize( _accountID, m_archivator, m_fileGroup, accountPath, projectVersion ) == false )
        {
            return nullptr;
        }

        m_invalidateAccounts = true;

        return newAccount;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AccountService::hasAccount( const ConstString& _accountID ) const
    {
        MapAccounts::const_iterator it_find = m_accounts.find( _accountID );

        if( it_find == m_accounts.end() )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AccountService::deleteAccount( const ConstString& _accountID )
    {
        MapAccounts::iterator it_find = m_accounts.find( _accountID );

        if( it_find == m_accounts.end() )
        {
            LOGGER_ERROR( "Can't delete account '%s'. There is no account with such ID"
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

        m_invalidateAccounts = true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AccountService::selectAccount( const ConstString& _accountID )
    {
        MapAccounts::iterator it_find = m_accounts.find( _accountID );

        if( it_find == m_accounts.end() )
        {
            LOGGER_ERROR( "Can't select account '%s'. There is no account with such ID"
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
    const AccountInterfacePtr & AccountService::getAccount( const ConstString& _accountID ) const
    {
        MapAccounts::const_iterator it_found = m_accounts.find( _accountID );

        if( it_found == m_accounts.end() )
        {
            LOGGER_ERROR( "account with ID '%s' not found"
                , _accountID.c_str()
            );

            return AccountInterfacePtr::none();
        }

        const AccountInterfacePtr & account = it_found->second;

        return account;
    }
    //////////////////////////////////////////////////////////////////////////
    void AccountService::visitAccounts( const AccountVisitorInterfacePtr & _visitor ) const
    {
        for( const MapAccounts::value_type & value : m_accounts )
        {
            const AccountInterfacePtr & account = value.second;

            const ConstString & accountID = account->getID();

            if( accountID == m_globalAccountID )
            {
                continue;
            }

            _visitor->onAccount( account );
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
        bool noLoadAccount = HAS_OPTION( "noaccounts" );

        if( noLoadAccount == true )
        {
            return true;
        }

        FilePath accountsPath = CONFIG_VALUE( "Game", "AccountsPath", STRINGIZE_FILEPATH_LOCAL( "accounts.ini" ) );

        if( m_fileGroup->existFile( accountsPath ) == false )
        {
            LOGGER_WARNING( "not exist accounts '%s'"
                , accountsPath.c_str()
            );

            return true;
        }

        IniUtil::IniStore ini;
        if( IniUtil::loadIni( ini, m_fileGroup, accountsPath ) == false )
        {
            LOGGER_ERROR( "parsing accounts failed '%s'"
                , accountsPath.c_str()
            );

            return false;
        }

        if( IniUtil::getIniValue( ini, "SETTINGS", "AccountEnumerator", m_playerEnumerator ) == false )
        {
            LOGGER_ERROR( "get AccountEnumerator failed '%s'"
                , accountsPath.c_str()
            );

            return false;
        }

        if( IniUtil::getIniValue( ini, "SETTINGS", "GlobalAccountID", m_globalAccountID ) == false )
        {
            LOGGER_INFO( "get GlobalAccountID failed '%s'"
                , accountsPath.c_str()
            );
        }

        if( IniUtil::getIniValue( ini, "SETTINGS", "DefaultAccountID", m_defaultAccountID ) == false )
        {
            LOGGER_INFO( "get DefaultAccountID failed '%s'"
                , accountsPath.c_str()
            );
        }

        ConstString selectAccountID;
        if( IniUtil::getIniValue( ini, "SETTINGS", "SelectAccountID", selectAccountID ) == false )
        {
            LOGGER_INFO( "get SelectAccountID failed '%s'"
                , accountsPath.c_str()
            );
        }

        VectorConstString values;
        if( IniUtil::getIniValue( ini, "ACCOUNTS", "Account", values ) == false )
        {
            LOGGER_INFO( "get ACCOUNTS failed '%s'"
                , accountsPath.c_str()
            );
        }

        AccountInterfacePtr validAccount = nullptr;

        for( const ConstString & accountID : values )
        {
            AccountInterfacePtr account = this->newAccount_( accountID );

            if( account == nullptr )
            {
                LOGGER_ERROR( "invalid create account %s"
                    , accountID.c_str()
                );

                return false;
            }

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
            LOGGER_INFO( "select account '%s'"
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
            LOGGER_INFO( "set default account '%s'"
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
            LOGGER_INFO( "invalid set any accounts"
            );
        }

        m_invalidateAccounts = true;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AccountService::saveAccounts()
    {
        if( m_invalidateAccounts == false )
        {
            return true;
        }

        FilePath accountsPath = CONFIG_VALUE( "Game", "AccountsPath", STRINGIZE_FILEPATH_LOCAL( "accounts.ini" ) );

        OutputStreamInterfacePtr file = FILE_SERVICE()
            ->openOutputFile( m_fileGroup, accountsPath, MENGINE_DOCUMENT_FUNCTION );

        if( file == nullptr )
        {
            LOGGER_ERROR( "can't open file for writing. Accounts '%s' settings not saved"
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

        String AccountEnumerator;
        Helper::unsignedToString( m_playerEnumerator, AccountEnumerator );

        IniUtil::writeIniSetting( file, "AccountEnumerator", AccountEnumerator );

        IniUtil::writeIniSection( file, "[ACCOUNTS]" );

        for( MapAccounts::iterator
            it = m_accounts.begin(),
            it_end = m_accounts.end();
            it != it_end;
            ++it )
        {
            const ConstString & accountID = it->first;

            IniUtil::writeIniSetting( file, "Account", accountID );
        }

        for( MapAccounts::iterator
            it = m_accounts.begin(),
            it_end = m_accounts.end();
            it != it_end;
            ++it )
        {
            const AccountInterfacePtr & account = it->second;

            if( account->save() == false )
            {
                LOGGER_ERROR( "invalid save account '%s:%s'"
                    , account->getID().c_str()
                    , account->getFolder().c_str()
                );

                return false;
            }
        }

        return true;
    }
}