#include "Account.h"

#include "Interface/FileServiceInterface.h"
#include "Interface/MemoryInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/StreamHelper.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/FileStreamHelper.h"
#include "Kernel/Stringalized.h"
#include "Kernel/PathString.h"
#include "Kernel/UID.h"
#include "Kernel/JSONHelper.h"
#include "Kernel/ContentHelper.h"
#include "Kernel/ConfigHelper.h"
#include "Kernel/UUIDv4.h"
#include "Kernel/UUIDv4Helper.h"
#include "Kernel/Hexadecimal.h"
#include "Kernel/MemoryCopy.h"

#include "Config/StdIntTypes.h"
#include "Config/StdString.h"

#ifndef MENGINE_ACCOUNT_SETTINGS_JSON_PATH
#define MENGINE_ACCOUNT_SETTINGS_JSON_PATH "settings.json"
#endif

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    // Initial account save format with PROJECT_VERSION and UID.
    static const int64_t ACCOUNT_SAVE_VERSION__TARDIGRADE = MENGINE_INT64_C(1);

    // Added SAVE_VERSION and PURCHASE_TOKEN.
    static const int64_t ACCOUNT_SAVE_VERSION__KAKAPO = MENGINE_INT64_C(2);
    //////////////////////////////////////////////////////////////////////////
    Account::Account()
        : m_projectVersion( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Account::~Account()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Account::initialize( const ConstString & _id, const ArchivatorInterfacePtr & _archivator, const FileGroupInterfacePtr & _fileGroup, const FilePath & _folderPath, uint32_t _projectVersion )
    {
        m_accountId = _id;
        m_archivator = _archivator;
        m_fileGroup = _fileGroup;
        m_folderName = _folderPath;
        m_projectVersion = _projectVersion;

        PathString settingsJSONPath;
        settingsJSONPath += m_folderName;
        settingsJSONPath += MENGINE_ACCOUNT_SETTINGS_JSON_PATH;

        FilePath settingsJSONPath_f = Helper::stringizeFilePath( settingsJSONPath );

        ContentInterfacePtr settingsJSONContent = Helper::makeFileContent( m_fileGroup, settingsJSONPath_f, MENGINE_DOCUMENT_FACTORABLE );

        m_settingsJSONContent = settingsJSONContent;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Account::finalize()
    {
        m_archivator = nullptr;
        m_fileGroup = nullptr;

        m_settingsJSONContent = nullptr;

        m_settings.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    bool Account::setup()
    {
        if( m_fileGroup->createDirectory( m_folderName ) == false )
        {
            LOGGER_ERROR( "account with ID '%s' failed create directory '%s'"
                , m_accountId.c_str()
                , m_folderName.c_str()
            );

            return false;
        }

        if( Helper::makeUID( AccountUID::size_data, m_uid.data ) == false )
        {
            LOGGER_ERROR( "account '%s' invalid make UID"
                , m_accountId.c_str()
            );

            return false;
        }

        if( Helper::makeUUIDv4( &m_purchaseToken, EHF_LOWERCASE ) == false )
        {
            LOGGER_ERROR( "account '%s' invalid make purchase token"
                , m_accountId.c_str()
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & Account::getAccountId() const
    {
        return m_accountId;
    }
    //////////////////////////////////////////////////////////////////////////
    void Account::setUID( const AccountUID & _uid )
    {
        m_uid = _uid;
    }
    //////////////////////////////////////////////////////////////////////////
    const AccountUID & Account::getUID() const
    {
        return m_uid;
    }
    //////////////////////////////////////////////////////////////////////////
    const UUIDv4 & Account::getPurchaseToken() const
    {
        return m_purchaseToken;
    }
    //////////////////////////////////////////////////////////////////////////
    const FilePath & Account::getFolderName() const
    {
        return m_folderName;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Account::addSetting( const ConstString & _setting, const Char * _defaultValue, const AccountSettingProviderInterfacePtr & _provider )
    {
        MapSettings::iterator it = m_settings.find( _setting );

        if( it != m_settings.end() )
        {
            LOGGER_ERROR( "account '%s' setting '%s' already exist"
                , m_accountId.c_str()
                , _setting.c_str()
            );

            return false;
        }

        Setting st;
        st.value.assign( _defaultValue );
        st.provider = _provider;

        m_settings.emplace( _setting, st );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Account::changeSetting( const ConstString & _setting, const Char * _value )
    {
        MapSettings::iterator it_found = m_settings.find( _setting );

        if( it_found == m_settings.end() )
        {
            LOGGER_ERROR( "account '%s' setting '%s' does not exist. Can't change"
                , m_accountId.c_str()
                , _setting.c_str()
            );

            return false;
        }

        Setting & st = it_found->second;

        st.value.assign( _value );

        if( this->save() == false )
        {
            LOGGER_ERROR( "account '%s' setting '%s' save failed"
                , m_accountId.c_str()
                , _setting.c_str()
            );
        }

        if( st.provider != nullptr )
        {
            st.provider->onChangeSetting( _value );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Account::getSetting( const ConstString & _setting, const Char ** const _value ) const
    {
        MapSettings::const_iterator it_found = m_settings.find( _setting );

        if( it_found == m_settings.end() )
        {
            LOGGER_ERROR( "account '%s' setting '%s' does not exist. Can't get"
                , m_accountId.c_str()
                , _setting.c_str()
            );

            return false;
        }

        const Setting & st = it_found->second;

        *_value = st.value.c_str();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Account::hasSetting( const ConstString & _setting, const Char ** const _value ) const
    {
        MapSettings::const_iterator it_found = m_settings.find( _setting );

        if( it_found == m_settings.end() )
        {
            return false;
        }

        if( _value != nullptr )
        {
            const Setting & st = it_found->second;

            *_value = st.value.c_str();
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Account::load()
    {
        ConfigInterfacePtr config = this->getLoadSettingConfig_();

        if( config == nullptr )
        {
            return false;
        }

        int64_t projectVersion;
        if( config->hasValue( "ACCOUNT", "PROJECT_VERSION", MENGINE_INT64_C(1), &projectVersion ) == false )
        {
            LOGGER_ERROR( "account '%s' failed read project version"
                , m_accountId.c_str()
            );

            return false;
        }

        if( m_projectVersion != projectVersion )
        {
            LOGGER_ERROR( "account '%s' failed invalid project version '%" MENGINE_PRId64 "' need '%u'"
                , m_accountId.c_str()
                , projectVersion
                , m_projectVersion
            );

            return false;
        }

        String uid;
        if( config->hasValue( "ACCOUNT", "UID", String(), &uid ) == false )
        {
            LOGGER_ERROR( "account '%s' failed not found uid"
                , m_accountId.c_str()
            );

            return false;
        }

        MENGINE_ASSERTION_FATAL( uid.size() == AccountUID::size_data, "account '%s' invalid uid '%s'"
            , m_accountId.c_str()
            , uid.c_str()
        );

        Helper::memoryCopy( m_uid.data, 0, uid.c_str(), 0, AccountUID::size_data );

        int64_t saveVersion;
        if( config->hasValue( "ACCOUNT", "SAVE_VERSION", ACCOUNT_SAVE_VERSION__TARDIGRADE, &saveVersion ) == false )
        {
            if( config->existValue( "ACCOUNT", "SAVE_VERSION" ) == true )
            {
                LOGGER_ERROR( "account '%s' invalid save version"
                    , m_accountId.c_str()
                );

                return false;
            }
        }

        UUIDv4 purchaseToken;

        switch( saveVersion )
        {
        case ACCOUNT_SAVE_VERSION__TARDIGRADE:
            {
                if( Helper::makeUUIDv4( &purchaseToken, EHF_LOWERCASE ) == false )
                {
                    LOGGER_ERROR( "account '%s' invalid make purchase token"
                        , m_accountId.c_str()
                    );

                    return false;
                }
            }break;
        case ACCOUNT_SAVE_VERSION__KAKAPO:
            {
                String purchaseTokenString;
                if( config->hasValue( "ACCOUNT", "PURCHASE_TOKEN", String(), &purchaseTokenString ) == false )
                {
                    LOGGER_ERROR( "account '%s' failed not found purchase token"
                        , m_accountId.c_str()
                    );

                    return false;
                }

                const Char * purchaseTokenValue = purchaseTokenString.c_str();
                size_t purchaseTokenSize = purchaseTokenString.size();

                if( Helper::parseUUIDv4( purchaseTokenValue, purchaseTokenSize, &purchaseToken ) == false )
                {
                    LOGGER_ERROR( "account '%s' invalid purchase token"
                        , m_accountId.c_str()
                    );

                    return false;
                }
            }break;
        default:
            {
                LOGGER_ERROR( "account '%s' unsupported save version '%" MENGINE_PRId64 "'"
                    , m_accountId.c_str()
                    , saveVersion
                );

                return false;
            }
        }

        m_purchaseToken = purchaseToken;

        LOGGER_INFO( "account", "load account '%s' projectVersion [%" MENGINE_PRId64 "] uid '%.20s' settings"
            , m_accountId.c_str()
            , projectVersion
            , m_uid.data
        );

        for( auto && [key, st] : m_settings )
        {
            String value;
            if( config->hasValue( "SETTINGS", key.c_str(), String(), &value ) == false )
            {
                continue;
            }

            LOGGER_INFO( "account", "load setting '%s' value '%s'"
                , key.c_str()
                , value.c_str()
            );

            st.value.assign( value );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    ConfigInterfacePtr Account::getLoadSettingConfig_() const
    {
        if( m_settingsJSONContent->exist( true ) == false )
        {
            LOGGER_ERROR( "account '%s' settings config not found '%s'"
                , m_accountId.c_str()
                , Helper::getContentFullPath( m_settingsJSONContent ).c_str()
            );

            return nullptr;
        }

        ConfigInterfacePtr config = Helper::loadConfig( m_settingsJSONContent, ConstString::none(), MENGINE_DOCUMENT_FACTORABLE );

        if( config == nullptr )
        {
            LOGGER_ERROR( "get account '%s' settings failed load '%s'"
                , m_accountId.c_str()
                , Helper::getContentFullPath( m_settingsJSONContent ).c_str()
            );

            return nullptr;
        }

        return config;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Account::save()
    {
        jpp::object j_root = jpp::make_object();

        jpp::object j_account = jpp::make_object();

        j_account.set( "SAVE_VERSION", ACCOUNT_SAVE_VERSION__KAKAPO );
        j_account.set( "PROJECT_VERSION", m_projectVersion );
        j_account.set( "UID", m_uid.data, AccountUID::size_data );
        j_account.set( "PURCHASE_TOKEN", m_purchaseToken.data, UUIDv4::size_data );

        LOGGER_INFO( "account", "save account '%s' projectVersion [%u] uid '%.20s' settings"
            , m_accountId.c_str()
            , m_projectVersion
            , m_uid.data
        );

        jpp::object j_settings = jpp::make_object();

        for( auto && [key, st] : m_settings )
        {
            const Char * value_str = st.value.c_str();

            LOGGER_INFO( "account", "save setting '%s' value '%s'"
                , key.c_str()
                , value_str
            );

            j_settings.set( key.c_str(), value_str );
        }

        j_root.set( "ACCOUNT", j_account );
        j_root.set( "SETTINGS", j_settings );

        const FilePath & settingsJSONPath = m_settingsJSONContent->getFilePath();

        if( Helper::writeJSONFile( j_root, m_fileGroup, settingsJSONPath, true, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            LOGGER_ERROR( "account '%s' invalid write json to file '%s'"
                , m_accountId.c_str()
                , Helper::getContentFullPath( m_settingsJSONContent ).c_str()
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Account::apply()
    {
        for( auto && [key, st] : m_settings )
        {
            if( st.provider == nullptr )
            {
                continue;
            }

            const Char * value_str = st.value.c_str();

            st.provider->onChangeSetting( value_str );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    InputStreamInterfacePtr Account::openReadBinaryFile( const FilePath & _filePath )
    {
        PathString path;
        path += m_folderName;
        path += _filePath;

        FilePath fullFilePath = Helper::stringizeFilePath( path );

        InputStreamInterfacePtr stream = Helper::openInputStreamFile( m_fileGroup, fullFilePath, false, false, MENGINE_DOCUMENT_FACTORABLE );

        if( stream == nullptr )
        {
            LOGGER_ERROR( "account '%s' invalid open file '%s'"
                , m_accountId.c_str()
                , fullFilePath.c_str()
            );

            return nullptr;
        }

        return stream;
    }
    //////////////////////////////////////////////////////////////////////////
    void Account::closeReadBinaryFile( const InputStreamInterfacePtr & _stream )
    {
        m_fileGroup->closeInputFile( _stream );
    }
    //////////////////////////////////////////////////////////////////////////
    OutputStreamInterfacePtr Account::openWriteBinaryFile( const FilePath & _filePath )
    {
        PathString path;
        path += m_folderName;
        path += _filePath;

        FilePath fullFilePath = Helper::stringizeFilePath( path );

        OutputStreamInterfacePtr stream = Helper::openOutputStreamFile( m_fileGroup, fullFilePath, true, MENGINE_DOCUMENT_FACTORABLE );

        if( stream == nullptr )
        {
            LOGGER_ERROR( "account '%s' invalid open file '%s'"
                , m_accountId.c_str()
                , _filePath.c_str()
            );

            return nullptr;
        }

        return stream;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Account::closeWriteBinaryFile( const OutputStreamInterfacePtr & _stream, bool _successful )
    {
        bool result = m_fileGroup->closeOutputFile( _stream, _successful );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    MemoryInterfacePtr Account::loadBinaryFile( const FilePath & _filePath )
    {
        InputStreamInterfacePtr stream = this->openReadBinaryFile( _filePath );

        if( stream == nullptr )
        {
            LOGGER_ERROR( "account '%s' invalid open file '%s'"
                , m_accountId.c_str()
                , _filePath.c_str()
            );

            return nullptr;
        }

        MemoryInterfacePtr binaryBuffer = Helper::readStreamArchiveMagic( stream, m_archivator, GET_MAGIC_NUMBER( MAGIC_ACCOUNT_DATA ), GET_MAGIC_VERSION( MAGIC_ACCOUNT_DATA ), MENGINE_DOCUMENT_FACTORABLE );

        this->closeReadBinaryFile( stream );

        if( binaryBuffer == nullptr )
        {
            LOGGER_ERROR( "account '%s' invalid load stream archive '%s'"
                , m_accountId.c_str()
                , _filePath.c_str()
            );

            return nullptr;
        }

        return binaryBuffer;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Account::writeBinaryFile( const FilePath & _filepath, const void * _data, size_t _size )
    {
        if( _data == nullptr || _size == 0 )
        {
            LOGGER_ERROR( "account '%s' write empty file '%s'"
                , m_accountId.c_str()
                , _filepath.c_str()
            );

            return false;
        }

        OutputStreamInterfacePtr stream = this->openWriteBinaryFile( _filepath );

        if( stream == nullptr )
        {
            LOGGER_ERROR( "account '%s' invalid open file '%s'"
                , m_accountId.c_str()
                , _filepath.c_str()
            );

            return false;
        }

        const void * data_memory = _data;
        size_t data_size = _size;

        bool successful = Helper::writeStreamArchiveMagic( stream, m_archivator, GET_MAGIC_NUMBER( MAGIC_ACCOUNT_DATA ), GET_MAGIC_VERSION( MAGIC_ACCOUNT_DATA ), true, data_memory, data_size, EAC_NORMAL );

        this->closeWriteBinaryFile( stream, successful );

        if( successful == false )
        {
            LOGGER_ERROR( "account '%s' invalid write file '%s'"
                , m_accountId.c_str()
                , _filepath.c_str()
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Account::hasBinaryFile( const FilePath & _filePath ) const
    {
        PathString path;
        path += m_folderName;
        path += _filePath;

        FilePath fullpath = Helper::stringizeFilePath( path );

        bool exist = m_fileGroup->existFile( fullpath, true );

        return exist;
    }
    //////////////////////////////////////////////////////////////////////////
}
